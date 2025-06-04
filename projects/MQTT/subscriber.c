#include "pico/cyw43_arch.h"
#include "pico/stdlib.h"
#include "pico/time.h"  
#include <stdio.h>
#include <string.h>

#include "lwip/apps/mqtt.h"
#include "lwipopts.h"
#include "lwip/netif.h"

void print_network_info() {
    struct netif *netif = netif_default;
    if (netif != NULL && netif_is_up(netif)) {
        printf("IP: %s\n", ip4addr_ntoa(&netif->ip_addr));
        printf("Máscara: %s\n", ip4addr_ntoa(&netif->netmask));
        printf("Gateway: %s\n", ip4addr_ntoa(&netif->gw));
    } else {
        printf("Interface de rede não está ativa!\n");
    }
}

static mqtt_client_t *client;

#define MAX_MSG_LEN 128
static char last_received_msg[MAX_MSG_LEN];
static volatile bool new_msg_received = false;

static int64_t last_msg_timestamp = -1;  // Timestamp da última mensagem recebida

void mqtt_incoming_publish_cb(void *arg, const char *topic, u32_t tot_len) {
    printf("Mensagem publicada no tópico: %s (%u bytes)\n", topic, tot_len);
}

void mqtt_incoming_data_cb(void *arg, const u8_t *data, u16_t len, u8_t flags) {
    static size_t received_len = 0;

    size_t copy_len = (received_len + len > MAX_MSG_LEN - 1) ? (MAX_MSG_LEN - 1 - received_len) : len;
    memcpy(&last_received_msg[received_len], data, copy_len);
    received_len += copy_len;

    if (flags & MQTT_DATA_FLAG_LAST) {
        last_received_msg[received_len] = '\0';

        for (size_t i = 0; i < received_len; i++) {
            last_received_msg[i] ^= 42;
        }

        // Pega timestamp atual
        absolute_time_t now = get_absolute_time();
        int64_t current_timestamp = to_ms_since_boot(now);

        if (last_msg_timestamp == current_timestamp) {
            printf("[ERRO] Replay detectado! Timestamp: %lld ms\n", current_timestamp);
        } else {
            printf("[OK] Nova mensagem recebida. Timestamp: %lld ms\n", current_timestamp);
        }

        last_msg_timestamp = current_timestamp;

        new_msg_received = true;

        received_len = 0;
    }
}

static void mqtt_connection_cb(mqtt_client_t *client, void *arg, mqtt_connection_status_t status) {
    if (status == MQTT_CONNECT_ACCEPTED) {
        printf("Conectado ao broker MQTT!\n");

        mqtt_set_inpub_callback(client, mqtt_incoming_publish_cb, mqtt_incoming_data_cb, NULL);

        err_t err = mqtt_subscribe(client, "escola/sala1/temperatura", 0, NULL, NULL);
        if (err != ERR_OK) {
            printf("Erro ao inscrever no tópico: %d\n", err);
        } else {
            printf("Inscrição no tópico realizada com sucesso!\n");
        }
    } else {
        printf("Falha ao conectar ao broker, código: %d\n", status);
    }
}

void mqtt_setup(const char *client_id, const char *broker_ip, const char *user, const char *pass) {
    ip_addr_t broker_addr;

    if (!ip4addr_aton(broker_ip, &broker_addr)) {
        printf("Erro no IP\n");
        return;
    }

    client = mqtt_client_new();
    if (client == NULL) {
        printf("Falha ao criar o cliente MQTT\n");
        return;
    }

    struct mqtt_connect_client_info_t ci = {
        .client_id = client_id,
        .client_user = user,
        .client_pass = pass
    };

    mqtt_client_connect(client, &broker_addr, 1883, mqtt_connection_cb, NULL, &ci);
}

static void mqtt_pub_request_cb(void *arg, err_t result) {
    if (result == ERR_OK) {
        printf("Publicação MQTT enviada com sucesso!\n");
    } else {
        printf("Erro ao publicar via MQTT: %d\n", result);
    }
}

void mqtt_comm_publish(const char *topic, const uint8_t *data, size_t len) {
    err_t status = mqtt_publish(
        client,
        topic,
        data,
        len,
        0,
        0,
        mqtt_pub_request_cb,
        NULL
    );

    if (status != ERR_OK) {
        printf("mqtt_publish falhou: %d\n", status);
    }
}

void connect_to_wifi(const char *ssid, const char *password) {
    if (cyw43_arch_init()) {
        printf("Erro ao iniciar Wi-Fi\n");
        return;
    }

    cyw43_arch_enable_sta_mode();

    if (cyw43_arch_wifi_connect_timeout_ms(ssid, password, CYW43_AUTH_WPA2_AES_PSK, 30000)) {
        printf("Erro ao conectar\n");
    } else {
        printf("Conectado ao Wi-Fi\n");
    }
}

void xor_encrypt(const uint8_t *input, uint8_t *output, size_t len, uint8_t key) {
    for (size_t i = 0; i < len; ++i) {
        output[i] = input[i] ^ key;
    }
}

int main() {
    stdio_init_all();
    sleep_ms(5000);

    connect_to_wifi("MARTINS WIFI-2.4", "20025450");
    sleep_ms(5000);

    print_network_info();

    mqtt_setup("bitdog", "192.168.15.146", "aluno", "2509");
    sleep_ms(3000);
    

    const char *mensagem = "26.5";
    uint8_t criptografada[16];
    xor_encrypt((uint8_t *)mensagem, criptografada, strlen(mensagem), 42);

    while (true) {
        cyw43_arch_poll();

        if (new_msg_received) {
            printf("Mensagem recebida no tópico: %s\n", last_received_msg);
            new_msg_received = false; 
        }

        sleep_ms(1000);
    }
}

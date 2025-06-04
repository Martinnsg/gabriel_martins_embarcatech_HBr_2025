#include "pico/cyw43_arch.h"           // Biblioteca para controle do chip 
#include "pico/stdlib.h"
#include "pico/time.h"                 // Para timestamp
#include <stdio.h>                     // Biblioteca padrão de entrada/saída (para usar printf)

#include "lwip/apps/mqtt.h"       // Biblioteca MQTT do lwIP
#include "lwipopts.h"             // Configurações customizadas do lwIP
#include "lwip/netif.h"

void print_network_info() {
    struct netif *netif = netif_default; // Obtém a interface de rede padrão
    
    if (netif != NULL && netif_is_up(netif)) {
        printf("IP: %s\n", ip4addr_ntoa(&netif->ip_addr));
        printf("Máscara: %s\n", ip4addr_ntoa(&netif->netmask));
        printf("Gateway: %s\n", ip4addr_ntoa(&netif->gw));
    } else {
        printf("Interface de rede não está ativa!\n");
    }
}

/* Variável global estática para armazenar a instância do cliente MQTT */
static mqtt_client_t *client;

/* Callback de conexão MQTT */
static void mqtt_connection_cb(mqtt_client_t *client, void *arg, mqtt_connection_status_t status) {
    if (status == MQTT_CONNECT_ACCEPTED) {
        printf("Conectado ao broker MQTT com sucesso!\n");
    } else {
        printf("Falha ao conectar ao broker, código: %d\n", status);
    }
}

/* Função para configurar e iniciar a conexão MQTT */
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

/* Callback de confirmação de publicação */
static void mqtt_pub_request_cb(void *arg, err_t result) {
    absolute_time_t now = get_absolute_time();
    int64_t timestamp_ms = to_ms_since_boot(now);

    if (result == ERR_OK) {
        printf("[%lld ms] Publicação MQTT enviada com sucesso!\n", timestamp_ms);
    } else {
        printf("[%lld ms] Erro ao publicar via MQTT: %d\n", timestamp_ms, result);
    }
}

/* Função para publicar dados em um tópico MQTT */
void mqtt_comm_publish(const char *topic, const uint8_t *data, size_t len) {
    err_t status = mqtt_publish(
        client,
        topic,
        data,
        len,
        0,   // QoS 0
        0,   // Não reter
        mqtt_pub_request_cb,
        NULL
    );

    absolute_time_t now = get_absolute_time();
    int64_t timestamp_ms = to_ms_since_boot(now);

    if (status != ERR_OK) {
        printf("[%lld ms] mqtt_publish falhou ao ser enviada: %d\n", now, status);
    } else {
        printf("[%lld ms] mqtt_publish enviada para o tópico: %s\n", now, topic);
    }
}

/* Função para conectar ao Wi-Fi */
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

/* Função de criptografia XOR */
void xor_encrypt(const uint8_t *input, uint8_t *output, size_t len, uint8_t key) {
    for (size_t i = 0; i < len; ++i) {
        output[i] = input[i] ^ key;
    }
}

int main()
{
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

    while (true)
    {
        cyw43_arch_poll();

        mqtt_comm_publish("escola/sala1/temperatura", (uint8_t *)"jao", strlen("jao"));

        sleep_ms(1000);
    }
}

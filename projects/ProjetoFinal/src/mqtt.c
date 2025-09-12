#include "include/mqtt.h"
#include "include/display.h"
#include "pico/cyw43_arch.h"
#include "pico/stdlib.h"
#include "pico/time.h"
#include <stdio.h>
#include <string.h>
#include "lwip/apps/mqtt.h"
#include "lwip/netif.h"
#include "include/lwipopts.h"

/* Variáveis globais */
float temp_min = 20.0, temp_max = 30.0;
float hum_min  = 40.0, hum_max  = 80.0;
float lum_min  = 100.0, lum_max = 800.0;
float soil_min = 30.0, soil_max = 70.0;

/* Cliente MQTT */
static mqtt_client_t *client;
static char current_topic[64];

/* Atualizações de thresholds */
void update_threshold_temp(const char *payload) {
    float min, max;
    if (sscanf(payload, "$THRESH:%f,%f", &min, &max) == 2) {
        temp_min = min;
        temp_max = max;
        printf("TEMP thresholds atualizados: min=%.2f max=%.2f\n", temp_min, temp_max);
        update_thresholds_display(temp_min, temp_max, hum_min, hum_max, soil_min, soil_max, lum_min, lum_max);
    }
}

void update_threshold_hum(const char *payload) {
    float min, max;
    if (sscanf(payload, "$THRESH:%f,%f", &min, &max) == 2) {
        hum_min = min;
        hum_max = max;
        printf("HUM thresholds atualizados: min=%.2f max=%.2f\n", hum_min, hum_max);
        update_thresholds_display(temp_min, temp_max, hum_min, hum_max, soil_min, soil_max, lum_min, lum_max);
    }
}

void update_threshold_lum(const char *payload) {
    float min, max;
    if (sscanf(payload, "$THRESH:%f,%f", &min, &max) == 2) {
        lum_min = min;
        lum_max = max;
        printf("LUM thresholds atualizados: min=%.2f max=%.2f\n", lum_min, lum_max);
        update_thresholds_display(temp_min, temp_max, hum_min, hum_max, soil_min, soil_max, lum_min, lum_max);
    }
}

void update_threshold_soil(const char *payload) {
    float min, max;
    if (sscanf(payload, "$THRESH:%f,%f", &min, &max) == 2) {
        soil_min = min;
        soil_max = max;
        printf("SOIL thresholds atualizados: min=%.2f max=%.2f\n", soil_min, soil_max);
        update_thresholds_display(temp_min, temp_max, hum_min, hum_max, soil_min, soil_max, lum_min, lum_max);
    }
}

/* Callbacks MQTT */
static void mqtt_sub_request_cb(void *arg, err_t result) {
    printf("Subscrição concluída com status %d\n", result);
}

static void mqtt_connection_cb(mqtt_client_t *client, void *arg, mqtt_connection_status_t status) {
    if (status == MQTT_CONNECT_ACCEPTED) {
        printf("Conectado ao broker MQTT!\n");
        mqtt_set_inpub_callback(client, mqtt_incoming_publish_cb, mqtt_incoming_data_cb, NULL);

        // Subscrições específicas de thresholds
        mqtt_subscribe(client, "bitdoglab2/temperatura", 0, mqtt_sub_request_cb, NULL);
        mqtt_subscribe(client, "bitdoglab2/luminosidade", 0, mqtt_sub_request_cb, NULL);
        mqtt_subscribe(client, "bitdoglab2/umidade/ar", 0, mqtt_sub_request_cb, NULL);
        mqtt_subscribe(client, "bitdoglab2/umidade/solo", 0, mqtt_sub_request_cb, NULL);
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
    if (!client) {
        printf("Falha ao criar cliente MQTT\n");
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
    absolute_time_t now = get_absolute_time();
    int64_t timestamp_ms = to_ms_since_boot(now);

    if (result == ERR_OK) {
        printf("[%lld ms] Publicação MQTT enviada com sucesso!\n", timestamp_ms);
    } else {
        printf("[%lld ms] Erro ao publicar via MQTT: %d\n", timestamp_ms, result);
    }
}

void mqtt_comm_publish(const char *topic, const uint8_t *data, size_t len) {
    err_t status = mqtt_publish(client, topic, data, len, 0, 0, mqtt_pub_request_cb, NULL);
    if (status != ERR_OK) {
        printf("mqtt_publish falhou: %d\n", status);
    }
}

/* Wi-Fi */
void connect_to_wifi(const char *ssid, const char *password) {
    if (cyw43_arch_init()) {
        printf("Erro ao iniciar Wi-Fi\n");
        return;
    }
    cyw43_arch_enable_sta_mode();
    if (cyw43_arch_wifi_connect_timeout_ms(ssid, password, CYW43_AUTH_WPA2_AES_PSK, 30000)) {
        printf("Erro ao conectar Wi-Fi\n");
    } else {
        printf("Conectado ao Wi-Fi!\n");
    }
}

void print_network_info(void) {
    struct netif *netif = netif_default;
    if (netif != NULL && netif_is_up(netif)) {
        printf("IP: %s\n", ip4addr_ntoa(&netif->ip_addr));
        printf("Máscara: %s\n", ip4addr_ntoa(&netif->netmask));
        printf("Gateway: %s\n", ip4addr_ntoa(&netif->gw));
    } else {
        printf("Interface de rede não está ativa!\n");
    }
}

/* Callbacks de publicação */
static void mqtt_incoming_publish_cb(void *arg, const char *topic, uint32_t tot_len) {
    strncpy(current_topic, topic, sizeof(current_topic)-1);
    current_topic[sizeof(current_topic)-1] = '\0';
}

static void mqtt_incoming_data_cb(void *arg, const uint8_t *data, uint16_t len, uint8_t flags) {
    char payload[128];
    if (len >= sizeof(payload)) len = sizeof(payload)-1;
    strncpy(payload, (const char *)data, len);
    payload[len] = '\0';

    // Só processa mensagens $THRESH
    if (strncmp(payload, "$THRESH", 7) != 0) return;

    if (strcmp(current_topic, "bitdoglab2/temperatura") == 0) {
        update_threshold_temp(payload);
    } else if (strcmp(current_topic, "bitdoglab2/umidade/ar") == 0) {
        update_threshold_hum(payload);
    } else if (strcmp(current_topic, "bitdoglab2/luminosidade") == 0) {
        update_threshold_lum(payload);
    } else if (strcmp(current_topic, "bitdoglab2/umidade/solo") == 0) {
        update_threshold_soil(payload);
    }
}

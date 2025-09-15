#ifndef MQTT_THRESHOLDS_H
#define MQTT_THRESHOLDS_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>
/* Variáveis globais dos thresholds */
extern float temp_min, temp_max;
extern float hum_min, hum_max;
extern float lum_min, lum_max;
extern float soil_min, soil_max;

/* Funções para atualizar thresholds de cada sensor */
void update_threshold_temp(const char *payload);
void update_threshold_hum(const char *payload);
void update_threshold_lum(const char *payload);
void update_threshold_soil(const char *payload);

/* Inicialização do MQTT */
void mqtt_setup(const char *client_id, const char *broker_ip, const char *user, const char *pass);
void mqtt_comm_publish(const char *topic, const char *msg);
/* Protótipos */
static void mqtt_incoming_publish_cb(void *arg, const char *topic, uint32_t tot_len);
static void mqtt_incoming_data_cb(void *arg, const uint8_t *data, uint16_t len, uint8_t flags);
/* Conectar Wi-Fi */
void connect_to_wifi(const char *ssid, const char *password);

/* Exibir informações da rede */
void print_network_info(void);

#endif // MQTT_THRESHOLDS_H

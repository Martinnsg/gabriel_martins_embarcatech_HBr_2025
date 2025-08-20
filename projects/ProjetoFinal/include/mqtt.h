#ifndef MQTT_H
#define MQTT_H

#include "pico/cyw43_arch.h"           // Biblioteca para controle do chip 
#include "pico/stdlib.h"
#include "pico/time.h"                 // Para timestamp
#include <stdio.h>                     // Biblioteca padrão de entrada/saída (para usar printf)

#include "lwip/apps/mqtt.h"       // Biblioteca MQTT do lwIP
#include "include/lwipopts.h"             // Configurações customizadas do lwIP
#include "lwip/netif.h"

void print_network_info();
static void mqtt_connection_cb(mqtt_client_t *client, void *arg, mqtt_connection_status_t status);
void mqtt_setup(const char *client_id, const char *broker_ip, const char *user, const char *pass);
static void mqtt_pub_request_cb(void *arg, err_t result);
void mqtt_comm_publish(const char *topic, const uint8_t *data, size_t len);
void connect_to_wifi(const char *ssid, const char *password);
void xor_encrypt(const uint8_t *input, uint8_t *output, size_t len, uint8_t key);

#endif

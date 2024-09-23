#ifndef _MQTT_LIB_H_
#define _MQTT_LIB_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "env.h"

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "protocol_examples_common.h"
#include "esp_log.h"
#include "mqtt_client.h"

    // MQTT Configurations
    static const char *TAG_MQTT = "mqtt";
    static esp_mqtt_client_handle_t client;

    // Function Declarations
    void setup_mqtt();
    void mqtt_publish(const char *topic, const char *data, int len, int qos, int retain);
    void mqtt_subscribe(char *topic, int qos);

    typedef void (*mqtt_on_message_callback_t)(char *topic, int topic_len, char *data, int data_len);
    typedef void (*mqtt_on_connect_callback_t)(void);

    void mqtt_set_on_message_callback(mqtt_on_message_callback_t callback);
    void mqtt_set_on_connect_callback(mqtt_on_connect_callback_t callback);

#ifdef __cplusplus
}
#endif

#endif /* _MQTT5_LIB_H_ */
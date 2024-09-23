#include "gateway.h"
#include "mqtt_lib.h"
#include "wifi_manager.h"
#include "http_server.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_log.h"

#define TOPIC_DEVICE DEVICE_ID

#define TOPIC_PPG TOPIC_DEVICE "ppg"

#define TOPIC_REGISTRATION "registration"

#define CONNECTED_RES "connected_ack"

static const char *TAG = "GATEWAY";

static bool device_registered = false;

void f_onMessageCallback(char *topic, int topic_len, char *data, int data_len)
{
    ESP_LOGI(TAG, "Message received on topic: %.*s", topic_len, topic);
    ESP_LOGI(TAG, "Data: %.*s", data_len, data);

    if (topic_len == strlen(TOPIC_DEVICE) && strncmp(topic, TOPIC_DEVICE, topic_len) == 0)
    {
        if (data_len == strlen(CONNECTED_RES) && strncmp(data, CONNECTED_RES, data_len) == 0)
        {
            device_registered = true;
        }
    }
}

void f_onConnectCallback(void)
{
    ESP_LOGI(TAG, "Connected to MQTT broker");

    mqtt_subscribe(TOPIC_DEVICE, 1);
}

void f_sendRegistrationRequest(void)
{
    mqtt_set_on_connect_callback(f_onConnectCallback);
    mqtt_set_on_message_callback(f_onMessageCallback);

    while (!device_registered)
    {
        mqtt_publish(TOPIC_REGISTRATION, DEVICE_ID, strlen(DEVICE_ID), 1, 0);

        vTaskDelay(5000);
    }
}

void f_setupGateway(void)
{
    ESP_LOGI(TAG, "Setting up gateway");

    wifi_manager_init(WIFI_MANAGER_MODE_APSTA);
    wifi_manager_start();

    start_webserver();

    while (!wifi_manager_is_connected_got_ip())
    {
        vTaskDelay(1000);
    }

    setup_mqtt();

    f_sendRegistrationRequest();

    ESP_LOGI(TAG, "Gateway setup completed successfully");
}

void f_transmitPacketPPG(uint32_t u4_index, uint32_t *pu4_irData, uint32_t *pu4_redData)
{
    if (!device_registered)
    {
        ESP_LOGE(TAG, "Device not registered yet");
        return;
    }

    packet_t packet;
    memset(&packet, 0, sizeof(packet));

    strncpy(packet.deviceId, DEVICE_ID, ID_DEVICE_LEN - 1);
    packet.deviceId[ID_DEVICE_LEN - 1] = '\0';
    packet.index = u4_index;
    memcpy(packet.irData, pu4_irData, DATA_LEN * sizeof(uint32_t));
    memcpy(packet.redData, pu4_redData, DATA_LEN * sizeof(uint32_t));

    mqtt_publish(TOPIC_PPG, (const char *)&packet, sizeof(packet), 1, 0);

    ESP_LOGI(TAG, "PPG data transmitted successfully");
}
#include "mqtt_lib.h"

// On message callback
static mqtt_on_message_callback_t mqtt_on_message_callback = NULL;

void mqtt_set_on_message_callback(mqtt_on_message_callback_t callback)
{
    mqtt_on_message_callback = callback;
}

void mqtt_on_message(char *topic, int topic_len, char *data, int data_len)
{
    if (mqtt_on_message_callback)
    {
        mqtt_on_message_callback(topic, topic_len, data, data_len);
    }
}

// On connect callback
static mqtt_on_connect_callback_t mqtt_on_connect_callback = NULL;

void mqtt_set_on_connect_callback(mqtt_on_connect_callback_t callback)
{
    mqtt_on_connect_callback = callback;
}

void mqtt_on_connect()
{
    if (mqtt_on_connect_callback)
    {
        mqtt_on_connect_callback();
    }
}

static void log_error_if_nonzero(const char *message, int error_code)
{
    if (error_code != 0)
    {
        ESP_LOGE(TAG_MQTT, "Last error %s: 0x%x", message, error_code);
    }
}

static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    ESP_LOGD(TAG_MQTT, "Event dispatched from event loop base=%s, event_id=%" PRIi32, base, event_id);
    esp_mqtt_event_handle_t event = event_data;
    esp_mqtt_client_handle_t client = event->client;

    switch ((esp_mqtt_event_id_t)event_id)
    {
    case MQTT_EVENT_CONNECTED:
        ESP_LOGI(TAG_MQTT, "MQTT_EVENT_CONNECTED");
        mqtt_on_connect();
        break;
    case MQTT_EVENT_DISCONNECTED:
        ESP_LOGI(TAG_MQTT, "MQTT_EVENT_DISCONNECTED");
        break;
    case MQTT_EVENT_SUBSCRIBED:
        ESP_LOGI(TAG_MQTT, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_UNSUBSCRIBED:
        ESP_LOGI(TAG_MQTT, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
        esp_mqtt_client_disconnect(client);
        break;
    case MQTT_EVENT_PUBLISHED:
        ESP_LOGI(TAG_MQTT, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_DATA:
        ESP_LOGI(TAG_MQTT, "MQTT_EVENT_DATA");
        ESP_LOGI(TAG_MQTT, "TOPIC: %.*s", event->topic_len, event->topic);
        ESP_LOGI(TAG_MQTT, "DATA: %.*s", event->data_len, event->data);

        mqtt_on_message(event->topic, event->topic_len, event->data, event->data_len);
        break;
    case MQTT_EVENT_ERROR:
        ESP_LOGI(TAG_MQTT, "MQTT_EVENT_ERROR");
        ESP_LOGI(TAG_MQTT, "MQTT return code: %d", event->error_handle->connect_return_code);
        if (event->error_handle->error_type == MQTT_ERROR_TYPE_TCP_TRANSPORT)
        {
            log_error_if_nonzero("reported from esp-tls", event->error_handle->esp_tls_last_esp_err);
            log_error_if_nonzero("reported from tls stack", event->error_handle->esp_tls_stack_err);
            log_error_if_nonzero("captured as transport's socket errno", event->error_handle->esp_transport_sock_errno);
            ESP_LOGI(TAG_MQTT, "Last errno string: %s", strerror(event->error_handle->esp_transport_sock_errno));
        }
        break;
    default:
        ESP_LOGI(TAG_MQTT, "Other event id: %d", event->event_id);
        break;
    }
}

static void mqtt_app_start()
{
    esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri = CONFIG_BROKER_URL,
        .credentials.username = USERNAME_MQTT_BROKER,                // ENV
        .credentials.authentication.password = PASSWORD_MQTT_BROKER, // ENV
    };

    client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
    esp_mqtt_client_start(client);
}

void setup_mqtt()
{
    ESP_LOGI(TAG_MQTT, "[APP] Startup..");
    ESP_LOGI(TAG_MQTT, "[APP] Free memory: %" PRIu32 " bytes", esp_get_free_heap_size());
    ESP_LOGI(TAG_MQTT, "[APP] IDF version: %s", esp_get_idf_version());

    esp_log_level_set("*", ESP_LOG_INFO);
    esp_log_level_set("mqtt_client", ESP_LOG_VERBOSE);
    esp_log_level_set("mqtt_example", ESP_LOG_VERBOSE);
    esp_log_level_set("transport_base", ESP_LOG_VERBOSE);
    esp_log_level_set("esp-tls", ESP_LOG_VERBOSE);
    esp_log_level_set("transport", ESP_LOG_VERBOSE);
    esp_log_level_set("outbox", ESP_LOG_VERBOSE);

    // ESP_ERROR_CHECK(nvs_flash_init());
    // ESP_ERROR_CHECK(esp_netif_init());
    // ESP_ERROR_CHECK(esp_event_loop_create_default());
    // ESP_ERROR_CHECK(example_connect());

    mqtt_app_start();
}

void mqtt_publish(const char *topic, const char *data, int len, int qos, int retain)
{
    int msg_id = esp_mqtt_client_publish(client, topic, data, len, qos, retain);
    ESP_LOGI(TAG_MQTT, "Sent publish successful, msg_id=%d", msg_id);
}

void mqtt_subscribe(char *topic, int qos)
{
    int msg_id = esp_mqtt_client_subscribe(client, topic, qos);
    ESP_LOGI(TAG_MQTT, "Sent subscribe successful, msg_id=%d", msg_id);
}

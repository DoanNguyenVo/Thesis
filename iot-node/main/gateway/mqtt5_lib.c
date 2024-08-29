#include "mqtt5_lib.h"

// On message callback
static mqtt5_on_message_callback_t mqtt5_on_message_callback = NULL;

void mqtt5_set_on_message_callback(mqtt5_on_message_callback_t callback)
{
    mqtt5_on_message_callback = callback;
}

void mqtt5_on_message(char *topic, int topic_len, char *data, int data_len)
{
    if (mqtt5_on_message_callback)
    {
        mqtt5_on_message_callback(topic, topic_len, data, data_len);
    }
}

// On connect callback
static mqtt5_on_connect_callback_t mqtt5_on_connect_callback = NULL;

void mqtt5_set_on_connect_callback(mqtt5_on_connect_callback_t callback)
{
    mqtt5_on_connect_callback = callback;
}

void mqtt5_on_connect()
{
    if (mqtt5_on_connect_callback)
    {
        mqtt5_on_connect_callback();
    }
}

// MQTT Property Configurations
static esp_mqtt5_publish_property_config_t publish_property = {
    .payload_format_indicator = 1,
    .message_expiry_interval = 1000,
    .response_topic = "/topic/test/response",
    .correlation_data = "123456",
    .correlation_data_len = 6,
};

static esp_mqtt5_subscribe_property_config_t subscribe_property = {
    .subscribe_id = 25555,
    .is_share_subscribe = true,
    .share_name = "group1",
};

static esp_mqtt5_disconnect_property_config_t disconnect_property = {
    .session_expiry_interval = 60,
    .disconnect_reason = 0,
};

static void log_error_if_nonzero(const char *message, int error_code)
{
    if (error_code != 0)
    {
        ESP_LOGE(TAG_MQTT, "Last error %s: 0x%x", message, error_code);
    }
}

static void print_user_property(mqtt5_user_property_handle_t user_property)
{
    if (user_property)
    {
        uint8_t count = esp_mqtt5_client_get_user_property_count(user_property);
        if (count)
        {
            esp_mqtt5_user_property_item_t *item = malloc(count * sizeof(esp_mqtt5_user_property_item_t));
            if (item && esp_mqtt5_client_get_user_property(user_property, item, &count) == ESP_OK)
            {
                for (int i = 0; i < count; i++)
                {
                    ESP_LOGI(TAG_MQTT, "key: %s, value: %s", item[i].key, item[i].value);
                }
                free(item);
            }
        }
    }
}

static void mqtt5_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    ESP_LOGD(TAG_MQTT, "Event dispatched from event loop base=%s, event_id=%" PRIi32, base, event_id);
    esp_mqtt_event_handle_t event = event_data;
    esp_mqtt_client_handle_t client = event->client;

    switch ((esp_mqtt_event_id_t)event_id)
    {
    case MQTT_EVENT_CONNECTED:
        ESP_LOGI(TAG_MQTT, "MQTT_EVENT_CONNECTED");
        print_user_property(event->property->user_property);
        mqtt5_on_connect();
        break;
    case MQTT_EVENT_DISCONNECTED:
        ESP_LOGI(TAG_MQTT, "MQTT_EVENT_DISCONNECTED");
        print_user_property(event->property->user_property);
        break;
    case MQTT_EVENT_SUBSCRIBED:
        ESP_LOGI(TAG_MQTT, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
        print_user_property(event->property->user_property);
        break;
    case MQTT_EVENT_UNSUBSCRIBED:
        ESP_LOGI(TAG_MQTT, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
        print_user_property(event->property->user_property);
        esp_mqtt5_client_set_user_property(&disconnect_property.user_property, user_property_arr, USE_PROPERTY_ARR_SIZE);
        esp_mqtt5_client_set_disconnect_property(client, &disconnect_property);
        esp_mqtt5_client_delete_user_property(disconnect_property.user_property);
        disconnect_property.user_property = NULL;
        esp_mqtt_client_disconnect(client);
        break;
    case MQTT_EVENT_PUBLISHED:
        ESP_LOGI(TAG_MQTT, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
        print_user_property(event->property->user_property);
        break;
    case MQTT_EVENT_DATA:
        ESP_LOGI(TAG_MQTT, "MQTT_EVENT_DATA");
        print_user_property(event->property->user_property);
        ESP_LOGI(TAG_MQTT, "payload_format_indicator: %d", event->property->payload_format_indicator);
        ESP_LOGI(TAG_MQTT, "response_topic: %.*s", event->property->response_topic_len, event->property->response_topic);
        ESP_LOGI(TAG_MQTT, "correlation_data: %.*s", event->property->correlation_data_len, event->property->correlation_data);
        ESP_LOGI(TAG_MQTT, "content_type: %.*s", event->property->content_type_len, event->property->content_type);
        ESP_LOGI(TAG_MQTT, "TOPIC: %.*s", event->topic_len, event->topic);
        ESP_LOGI(TAG_MQTT, "DATA: %.*s", event->data_len, event->data);

        mqtt5_on_message(event->topic, event->topic_len, event->data, event->data_len);
        break;
    case MQTT_EVENT_ERROR:
        ESP_LOGI(TAG_MQTT, "MQTT_EVENT_ERROR");
        print_user_property(event->property->user_property);
        ESP_LOGI(TAG_MQTT, "MQTT5 return code: %d", event->error_handle->connect_return_code);
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

static void mqtt5_app_start()
{
    esp_mqtt5_connection_property_config_t connect_property = {
        .session_expiry_interval = 10,
        .maximum_packet_size = 1024,
        .receive_maximum = 65535,
        .topic_alias_maximum = 2,
        .request_resp_info = true,
        .request_problem_info = true,
        .will_delay_interval = 10,
        .payload_format_indicator = true,
        .message_expiry_interval = 10,
        .response_topic = "/test/response",
        .correlation_data = "123456",
        .correlation_data_len = 6,
    };

    esp_mqtt_client_config_t mqtt5_cfg = {
        .broker.address.uri = CONFIG_BROKER_URL,
        .session.protocol_ver = MQTT_PROTOCOL_V_5,
        .credentials.username = USERNAME_MQTT_BROKER,                // ENV
        .credentials.authentication.password = PASSWORD_MQTT_BROKER, // ENV
        .session.last_will.topic = "/topic/test",
        .session.last_will.msg = "connected",
        .session.last_will.msg_len = 12,
        .session.last_will.qos = 1,
        .session.last_will.retain = true,
    };

    client = esp_mqtt_client_init(&mqtt5_cfg);
    esp_mqtt5_client_set_user_property(&connect_property.user_property, user_property_arr, USE_PROPERTY_ARR_SIZE);
    esp_mqtt5_client_set_user_property(&connect_property.will_user_property, user_property_arr, USE_PROPERTY_ARR_SIZE);
    esp_mqtt5_client_set_connect_property(client, &connect_property);
    esp_mqtt5_client_delete_user_property(connect_property.user_property);
    esp_mqtt5_client_delete_user_property(connect_property.will_user_property);
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt5_event_handler, NULL);
    esp_mqtt_client_start(client);
}

void setup_mqtt5()
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

    mqtt5_app_start();
}

void mqtt5_publish(const char *topic, const char *data, int len, int qos, int retain)
{
    esp_mqtt5_client_set_user_property(&publish_property.user_property, user_property_arr, USE_PROPERTY_ARR_SIZE);
    esp_mqtt5_client_set_publish_property(client, &publish_property);
    int msg_id = esp_mqtt_client_publish(client, topic, data, len, qos, retain);
    esp_mqtt5_client_delete_user_property(publish_property.user_property);
    publish_property.user_property = NULL;
    ESP_LOGI(TAG_MQTT, "Sent publish successful, msg_id=%d", msg_id);
}

void mqtt5_subscribe(char *topic, int qos)
{
    esp_mqtt5_client_set_user_property(&subscribe_property.user_property, user_property_arr, USE_PROPERTY_ARR_SIZE);
    esp_mqtt5_client_set_subscribe_property(client, &subscribe_property);
    int msg_id = esp_mqtt_client_subscribe(client, topic, qos);
    esp_mqtt5_client_delete_user_property(subscribe_property.user_property);
    subscribe_property.user_property = NULL;
    ESP_LOGI(TAG_MQTT, "Sent subscribe successful, msg_id=%d", msg_id);
}

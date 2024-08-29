#include "wifi_manager.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "lwip/err.h"
#include "lwip/sys.h"
#include "freertos/event_groups.h"
#include "esp_netif.h"

#define TAG "wifi_manager"

#define MAX_RETRY_COUNT 5
static int s_retry_num = 0;

static wifi_manager_mode_t current_mode;
static EventGroupHandle_t wifi_event_group;
static esp_netif_t *sta_netif = NULL;
static esp_netif_t *ap_netif = NULL;

static void wifi_event_handler(void *arg, esp_event_base_t event_base,
                               int32_t event_id, void *event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START)
    {
        esp_wifi_connect();
    }
    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED)
    {
        if (s_retry_num < MAX_RETRY_COUNT)
        {
            esp_wifi_connect();
            s_retry_num++;
            ESP_LOGI(TAG, "retry to connect to the AP");
        }
        else
        {
            xEventGroupSetBits(wifi_event_group, WIFI_FAIL_BIT);
        }
        ESP_LOGI(TAG, "connect to the AP fail");
    }
    else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP)
    {
        ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
        ESP_LOGI(TAG, "Got IP:" IPSTR, IP2STR(&event->ip_info.ip));
        s_retry_num = 0;
        xEventGroupSetBits(wifi_event_group, WIFI_CONNECTED_BIT);
    }
}

esp_err_t wifi_manager_init(wifi_manager_mode_t mode)
{
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    wifi_event_group = xEventGroupCreate();
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    current_mode = mode;

    if (mode == WIFI_MANAGER_MODE_STA || mode == WIFI_MANAGER_MODE_APSTA)
    {
        sta_netif = esp_netif_create_default_wifi_sta();
    }

    if (mode == WIFI_MANAGER_MODE_AP || mode == WIFI_MANAGER_MODE_APSTA)
    {
        ap_netif = esp_netif_create_default_wifi_ap();
    }

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &wifi_event_handler, NULL));

    return ESP_OK;
}

esp_err_t wifi_manager_start(void)
{
    wifi_config_t wifi_config = {0};

    if (current_mode == WIFI_MANAGER_MODE_STA || current_mode == WIFI_MANAGER_MODE_APSTA)
    {
        ESP_ERROR_CHECK(esp_wifi_set_mode(current_mode == WIFI_MANAGER_MODE_STA ? WIFI_MODE_STA : WIFI_MODE_APSTA));
        ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));
    }

    if (current_mode == WIFI_MANAGER_MODE_AP || current_mode == WIFI_MANAGER_MODE_APSTA)
    {
        strcpy((char *)wifi_config.ap.ssid, "ESP32-Config");
        wifi_config.ap.ssid_len = strlen("ESP32-Config");
        strcpy((char *)wifi_config.ap.password, "password");
        wifi_config.ap.max_connection = 4;
        wifi_config.ap.authmode = WIFI_AUTH_WPA_WPA2_PSK;

        ESP_ERROR_CHECK(esp_wifi_set_mode(current_mode == WIFI_MANAGER_MODE_AP ? WIFI_MODE_AP : WIFI_MODE_APSTA));
        ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_AP, &wifi_config));
    }

    ESP_ERROR_CHECK(esp_wifi_start());

    return ESP_OK;
}

esp_err_t wifi_manager_stop(void)
{
    ESP_ERROR_CHECK(esp_wifi_stop());
    return ESP_OK;
}

esp_err_t wifi_manager_connect_sta(const char *ssid, const char *password)
{
    s_retry_num = 0;
    if (strlen(ssid) == 0)
    {
        ESP_LOGE(TAG, "SSID is empty");
        return ESP_ERR_INVALID_ARG;
    }

    wifi_config_t wifi_config = {0};
    strncpy((char *)wifi_config.sta.ssid, ssid, sizeof(wifi_config.sta.ssid) - 1);
    strncpy((char *)wifi_config.sta.password, password, sizeof(wifi_config.sta.password) - 1);

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_APSTA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_connect());

    xEventGroupClearBits(wifi_event_group, WIFI_CONNECTED_BIT | WIFI_FAIL_BIT);
    EventBits_t bits = xEventGroupWaitBits(wifi_event_group,
                                           WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
                                           pdTRUE,
                                           pdFALSE,
                                           WIFI_CONNECT_TIMEOUT_MS / portTICK_PERIOD_MS);

    if (bits & WIFI_CONNECTED_BIT)
    {
        ESP_LOGI(TAG, "Connected to AP SSID:%s", ssid);
        return ESP_OK;
    }
    else if (bits & WIFI_FAIL_BIT)
    {
        ESP_LOGI(TAG, "Failed to connect to SSID:%s", ssid);
        // Disconnect and stop trying to connect
        esp_wifi_disconnect();
        esp_wifi_stop();
        esp_wifi_start();
        // Restore AP mode only
        ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
        return ESP_FAIL;
    }
    else
    {
        ESP_LOGE(TAG, "UNEXPECTED EVENT");
        // Restore AP mode only
        ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
        return ESP_ERR_TIMEOUT;
    }
}

esp_err_t wifi_manager_get_sta_ip(char *ip_address, size_t max_len)
{
    esp_netif_ip_info_t ip_info;
    ESP_ERROR_CHECK(esp_netif_get_ip_info(sta_netif, &ip_info));
    snprintf(ip_address, max_len, IPSTR, IP2STR(&ip_info.ip));
    return ESP_OK;
}

bool wifi_manager_is_connected_got_ip(void)
{
    if (sta_netif == NULL)
    {
        return false;
    }

    esp_netif_ip_info_t ip_info;
    if (esp_netif_get_ip_info(sta_netif, &ip_info) != ESP_OK)
    {
        return false;
    }

    return ip_info.ip.addr != 0;
}
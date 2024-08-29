#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "esp_wifi.h"
#include "esp_event.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"

#define MAX_SSID_LEN 32
#define MAX_PASSWORD_LEN 64

#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT BIT1
#define WIFI_CONNECT_TIMEOUT_MS 5000

    typedef enum
    {
        WIFI_MANAGER_MODE_STA,
        WIFI_MANAGER_MODE_AP,
        WIFI_MANAGER_MODE_APSTA
    } wifi_manager_mode_t;

    esp_err_t wifi_manager_init(wifi_manager_mode_t mode);
    esp_err_t wifi_manager_start(void);
    esp_err_t wifi_manager_stop(void);
    esp_err_t wifi_manager_connect_sta(const char *ssid, const char *password);
    esp_err_t wifi_manager_get_sta_ip(char *ip_address, size_t max_len);
    bool wifi_manager_is_connected_got_ip(void);

#ifdef __cplusplus
}
#endif

#endif // WIFI_MANAGER_H
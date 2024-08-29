#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "esp_http_server.h"

    esp_err_t start_webserver(void);
    void stop_webserver(httpd_handle_t server);

#ifdef __cplusplus
}
#endif

#endif // HTTP_SERVER_H

#include "http_server.h"
#include "esp_log.h"
#include "cJSON.h"
#include "esp_mac.h"
#include "wifi_manager.h"
#include <string.h>

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define TAG "http_server"

static esp_err_t root_get_handler(httpd_req_t *req)
{
    const char *resp_str = R"(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>ESP32 WiFi Config</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            background-color: #f0f0f0;
            margin: 0;
            padding: 0;
            display: flex;
            justify-content: center;
            align-items: center;
            height: 100vh;
        }
        .container {
            background-color: white;
            padding: 2rem;
            border-radius: 8px;
            box-shadow: 0 4px 6px rgba(0, 0, 0, 0.1);
            width: 100%;
            max-width: 350px;
        }
        h1 {
            color: #333;
            text-align: center;
            margin-bottom: 1.5rem;
        }
        form {
            display: flex;
            flex-direction: column;
        }
        label {
            margin-bottom: 0.5rem;
            color: #666;
        }
        input {
            padding: 0.5rem;
            margin-bottom: 1rem;
            border: 1px solid #ddd;
            border-radius: 4px;
            font-size: 1rem;
        }
        button {
            background-color: #4CAF50;
            color: white;
            padding: 0.75rem;
            border: none;
            border-radius: 4px;
            font-size: 1rem;
            cursor: pointer;
            transition: background-color 0.3s;
        }
        button:hover {
            background-color: #45a049;
        }
        #status {
            margin-top: 1rem;
            text-align: center;
            color: #666;
        }
        .loader {
            border: 4px solid #f3f3f3;
            border-top: 4px solid #3498db;
            border-radius: 50%;
            width: 20px;
            height: 20px;
            animation: spin 1s linear infinite;
            margin: 10px auto;
            display: none;
        }
        @keyframes spin {
            0% { transform: rotate(0deg); }
            100% { transform: rotate(360deg); }
        }
    </style>
</head>
<body>
    <div class="container">
        <h1>ESP32 WiFi Config</h1>
        <form id="wifi-form" enctype="multipart/form-data">
            <label for="ssid">SSID:</label>
            <input type="text" id="ssid" name="ssid" required>
            <label for="password">Password:</label>
            <input type="password" id="password" name="password">
            <button type="submit">Connect</button>
        </form>
        <div class="loader" id="loader"></div>
        <div id="status"></div>
    </div>
    <script>
        document.getElementById('wifi-form').onsubmit = function(e) {
            e.preventDefault();
            var formData = new FormData(e.target);
            var statusDiv = document.getElementById('status');
            var loader = document.getElementById('loader');
            
            statusDiv.textContent = '';
            loader.style.display = 'block';
            
            fetch('/connect', {method: 'POST', body: formData})
                .then(response => response.json())
                .then(data => {
                    loader.style.display = 'none';
                    statusDiv.textContent = data.message;
                })
                .catch(error => {
                    loader.style.display = 'none';
                    statusDiv.textContent = 'An error occurred: ' + error;
                });
        };
    </script>
</body>
</html>
    )";

    httpd_resp_set_type(req, "text/html");
    httpd_resp_send(req, resp_str, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

static esp_err_t connect_post_handler(httpd_req_t *req)
{
    char *buf = malloc(req->content_len + 1);
    if (buf == NULL)
    {
        return ESP_FAIL;
    }
    int ret = httpd_req_recv(req, buf, req->content_len);
    if (ret <= 0)
    {
        free(buf);
        return ESP_FAIL;
    }
    buf[req->content_len] = '\0';

    ESP_LOGI(TAG, "Received POST data: %s", buf);

    char ssid[MAX_SSID_LEN] = {0};
    char password[MAX_PASSWORD_LEN] = {0};

    // Parse multipart form data
    char *line = strtok(buf, "\r\n");
    while (line != NULL)
    {
        if (strstr(line, "name=\"ssid\"") != NULL)
        {
            line = strtok(NULL, "\r\n");
            if (line)
            {
                strncpy(ssid, line, MAX_SSID_LEN - 1);
            }
        }
        else if (strstr(line, "name=\"password\"") != NULL)
        {
            line = strtok(NULL, "\r\n");
            if (line)
            {
                strncpy(password, line, MAX_PASSWORD_LEN - 1);
            }
        }
        line = strtok(NULL, "\r\n");
    }

    ESP_LOGI(TAG, "Received SSID: %s", ssid);
    ESP_LOGI(TAG, "Received Password: %s", "********"); // Don't log actual password

    if (strlen(ssid) == 0)
    {
        ESP_LOGE(TAG, "SSID is empty");
        free(buf);
        return ESP_FAIL;
    }

    esp_err_t err = wifi_manager_connect_sta(ssid, password);

    cJSON *root = cJSON_CreateObject();
    if (err == ESP_OK)
    {
        cJSON_AddStringToObject(root, "message", "Connected successfully.");
    }
    else if (err == ESP_FAIL)
    {
        cJSON_AddStringToObject(root, "message", "Failed to connect to WiFi. Please check your credentials and try again.");
    }
    else if (err == ESP_ERR_TIMEOUT)
    {
        cJSON_AddStringToObject(root, "message", "Connection timeout. Please try again.");
    }
    else
    {
        cJSON_AddStringToObject(root, "message", "An error occurred during connection");
    }

    char *json_response = cJSON_Print(root);
    httpd_resp_set_type(req, "application/json");

    esp_err_t res = httpd_resp_send(req, json_response, HTTPD_RESP_USE_STRLEN);
    if (res != ESP_OK)
    {
        ESP_LOGE(TAG, "Error sending response: %d", res);
    }

    free(json_response);
    cJSON_Delete(root);
    free(buf);

    return ESP_OK;
}

esp_err_t start_webserver(void)
{
    httpd_handle_t server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();

    esp_err_t ret = httpd_start(&server, &config);
    if (ret != ESP_OK)
    {
        ESP_LOGI(TAG, "Error starting server!");
        return ret;
    }

    httpd_uri_t root = {
        .uri = "/",
        .method = HTTP_GET,
        .handler = root_get_handler,
        .user_ctx = NULL};
    httpd_register_uri_handler(server, &root);

    httpd_uri_t connect = {
        .uri = "/connect",
        .method = HTTP_POST,
        .handler = connect_post_handler,
        .user_ctx = NULL};
    httpd_register_uri_handler(server, &connect);

    return ESP_OK;
}

void stop_webserver(httpd_handle_t server)
{
    if (server)
    {
        httpd_stop(server);
    }
}
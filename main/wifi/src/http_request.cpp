/* HTTP GET Example using plain POSIX sockets

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "esp_tls.h"

#include "esp_http_client.h"
#include "uart_config.h"

#define MAX_HTTP_RECV_BUFFER 512
#define MAX_HTTP_OUTPUT_BUFFER 2048
static const char *TAG = "HTTP_CLIENT";

esp_err_t _http_event_handler(esp_http_client_event_t *evt)
{
    static char *output_buffer;  // Buffer to store response of http request from event handler
    static int output_len;       // Stores number of bytes read
    switch(evt->event_id) {
        case HTTP_EVENT_ERROR:
            uart_write_bytes(UART_OBD_PORT_NUM, "HTTP_EVENT_ERROR\r\n", 18);
            vTaskDelay(500 / portTICK_PERIOD_MS);
            ESP_LOGD(TAG, "HTTP_EVENT_ERROR");
            break;
        case HTTP_EVENT_ON_CONNECTED:
            uart_write_bytes(UART_OBD_PORT_NUM, "HTTP_EVENT_ON_CONNECTED\r\n", 25);
            vTaskDelay(500 / portTICK_PERIOD_MS);
            ESP_LOGD(TAG, "HTTP_EVENT_ON_CONNECTED");
            break;
        case HTTP_EVENT_HEADER_SENT:
            uart_write_bytes(UART_OBD_PORT_NUM, "HTTP_EVENT_HEADER_SENT\r\n", 24);
            vTaskDelay(500 / portTICK_PERIOD_MS);
            ESP_LOGD(TAG, "HTTP_EVENT_HEADER_SENT");
            break;
        case HTTP_EVENT_ON_HEADER:
            uart_write_bytes(UART_OBD_PORT_NUM, "HTTP_EVENT_ON_HEADER\r\n", 22);
            vTaskDelay(500 / portTICK_PERIOD_MS);
            ESP_LOGD(TAG, "HTTP_EVENT_ON_HEADER, key=%s, value=%s", evt->header_key, evt->header_value);
            break;
        case HTTP_EVENT_ON_DATA:
            uart_write_bytes(UART_OBD_PORT_NUM, "HTTP_EVENT_ON_DATA\r\n", 20);
            vTaskDelay(500 / portTICK_PERIOD_MS);
            ESP_LOGD(TAG, "HTTP_EVENT_ON_DATA, len=%d", evt->data_len);
            /*
             *  Check for chunked encoding is added as the URL for chunked encoding used in this example returns binary data.
             *  However, event handler can also be used in case chunked encoding is used.
             */
            if (!esp_http_client_is_chunked_response(evt->client)) {
                // If user_data buffer is configured, copy the response into the buffer
                if (evt->user_data) {
                    memcpy(evt->user_data + output_len, evt->data, evt->data_len);
                } else {
                    if (output_buffer == NULL) {
                        output_buffer = (char *) malloc(esp_http_client_get_content_length(evt->client));
                        output_len = 0;
                        if (output_buffer == NULL) {
                            uart_write_bytes(UART_OBD_PORT_NUM, "Failed to allocate memory for output buffer\r\n", 45);
                            vTaskDelay(500 / portTICK_PERIOD_MS);
                            ESP_LOGE(TAG, "Failed to allocate memory for output buffer");
                            return ESP_FAIL;
                        }
                    }
                    memcpy(output_buffer + output_len, evt->data, evt->data_len);
                }
                output_len += evt->data_len;
            }

            break;
        case HTTP_EVENT_ON_FINISH:
            uart_write_bytes(UART_OBD_PORT_NUM, "HTTP_EVENT_ON_FINISH\r\n", 22);
            vTaskDelay(500 / portTICK_PERIOD_MS);
            ESP_LOGD(TAG, "HTTP_EVENT_ON_FINISH");
            if (output_buffer != NULL) {
                // Response is accumulated in output_buffer. Uncomment the below line to print the accumulated response
                // ESP_LOG_BUFFER_HEX(TAG, output_buffer, output_len);
                free(output_buffer);
                output_buffer = NULL;
            }
            output_len = 0;
            break;
        case HTTP_EVENT_DISCONNECTED:
            uart_write_bytes(UART_OBD_PORT_NUM, "HTTP_EVENT_DISCONNECTED\r\n", 25);
            vTaskDelay(500 / portTICK_PERIOD_MS);
            ESP_LOGI(TAG, "HTTP_EVENT_DISCONNECTED");
            // int mbedtls_err = 0;
            // esp_err_t err = esp_tls_get_and_clear_last_error(evt->data, &mbedtls_err, NULL);
            // if (err != 0) {
            //     if (output_buffer != NULL) {
            //         free(output_buffer);
            //         output_buffer = NULL;
            //     }
            //     output_len = 0;
            //     ESP_LOGI(TAG, "Last esp error code: 0x%x", err);
            //     ESP_LOGI(TAG, "Last mbedtls failure: 0x%x", mbedtls_err);
            // }
            break;
    }
    return ESP_OK;
}

char * http_rest_with_url(bool request, const char* url)
{
    char local_response_buffer[MAX_HTTP_OUTPUT_BUFFER] = {0};
    char* ch = local_response_buffer;
    /**
     * NOTE: All the configuration parameters for http_client must be spefied either in URL or as host and path parameters.
     * If host and path parameters are not set, query parameter will be ignored. In such cases,
     * query parameter should be specified in URL.
     *
     * If URL as well as host and path parameters are specified, values of host and path will be considered.
     */
    esp_http_client_config_t config = {
        .url = url,
       // .path = "",
        .query = "esp",
        .disable_auto_redirect = false,
        .event_handler = _http_event_handler,
        .user_data = local_response_buffer        // Pass address of local buffer to get response
    };
    esp_http_client_handle_t client = esp_http_client_init(&config);
    uart_write_bytes(UART_OBD_PORT_NUM, "set wifi config\r\n", 17);
    vTaskDelay(5000 / portTICK_PERIOD_MS);

    if (!request) {
        // GET
        esp_err_t err = esp_http_client_perform(client);
        if (err == ESP_OK) {
            uart_write_bytes(UART_OBD_PORT_NUM, "get ok\r\n", 8);
            vTaskDelay(500 / portTICK_PERIOD_MS);
            ESP_LOGI(TAG, "HTTP GET Status = %d, content_length = %d",
                    esp_http_client_get_status_code(client),
                    esp_http_client_get_content_length(client));
        } else {
            uart_write_bytes(UART_OBD_PORT_NUM, "HTTP GET request failed\r\n", 25);
            vTaskDelay(500 / portTICK_PERIOD_MS);
            ESP_LOGE(TAG, "HTTP GET request failed: %s", esp_err_to_name(err));
        }
        //uart_write_bytes(UART_OBD_PORT_NUM, "response:\r\n", 11);
        vTaskDelay(500 / portTICK_PERIOD_MS);
    } 
    else {
        // POST
        uart_write_bytes(UART_OBD_PORT_NUM, "inside post\r\n", 13);
        //const char *post_data = "?id=1&latitude=20.999999&longitude=20.999999&isActive=1&dateOfLastStart=2022-09-25&timeOfLastStart=20:20&vehicleSpeed=40&fuelLevel=40.5&isMotion=0";
        //esp_http_client_set_url(client, "http://car-protector.herokuapp.com/postData");
        uart_write_bytes(UART_OBD_PORT_NUM, "set url\r\n", 9);
        vTaskDelay(200 / portTICK_PERIOD_MS);
        esp_http_client_set_method(client, HTTP_METHOD_POST);
        uart_write_bytes(UART_OBD_PORT_NUM, "set method\r\n", 12);
        vTaskDelay(200 / portTICK_PERIOD_MS);
        // esp_http_client_set_header(client, "Content-Type", "application/x-www-form-urlencoded");
        // uart_write_bytes(UART_OBD_PORT_NUM, "set content\r\n", 13);
        // vTaskDelay(200 / portTICK_PERIOD_MS);
        // esp_http_client_set_post_field(client, post_data, strlen(post_data));
        uart_write_bytes(UART_OBD_PORT_NUM, "set field\r\n", 11);
        vTaskDelay(200 / portTICK_PERIOD_MS);
        esp_err_t err = esp_http_client_perform(client);
        uart_write_bytes(UART_OBD_PORT_NUM, "performed\r\n", 11);
        vTaskDelay(200 / portTICK_PERIOD_MS);
        if (err == ESP_OK) {
            uart_write_bytes(UART_OBD_PORT_NUM, "post ok\r\n", 9);
            ESP_LOGI(TAG, "HTTP POST Status = %d, content_length = %d",
                    esp_http_client_get_status_code(client),
                    esp_http_client_get_content_length(client));
        } else {
            uart_write_bytes(UART_OBD_PORT_NUM, "HTTP POST request failed\r\n", 26);
            ESP_LOGE(TAG, "HTTP POST request failed: %s", esp_err_to_name(err));
        }
    }
    
    int i=0;
    // while(local_response_buffer[i] != 0) {        
    //     uart_write_bytes(UART_OBD_PORT_NUM, &local_response_buffer[i], 1);
    //     ++i;
    // }
    vTaskDelay(500 / portTICK_PERIOD_MS);
    //ESP_LOG_BUFFER_HEX(TAG, local_response_buffer, strlen(local_response_buffer));

    esp_http_client_cleanup(client);
    return ch;
}
/* WiFi station Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include "uart_talk.h"

#include "esp_event.h"
#include "esp_log.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "freertos/event_groups.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "lwip/err.h"
#include "lwip/sys.h"
#include "nvs_flash.h"

#include <string.h>

/* The examples use WiFi configuration that you can set via project configuration menu

   If you'd rather not, just change the below entries to strings with
   the config you want - ie #define EXAMPLE_WIFI_SSID "mywifissid"
*/
#define EXAMPLE_ESP_WIFI_SSID      "NETIASPOT-h6hN"
#define EXAMPLE_ESP_WIFI_PASS      "MpbC6se7We9Pc"
#define EXAMPLE_ESP_MAXIMUM_RETRY  3

/* FreeRTOS event group to signal when we are connected*/
static EventGroupHandle_t s_wifi_event_group;

/* The event group allows multiple bits for each event, but we only care about two events:
 * - we are connected to the AP with an IP
 * - we failed to connect after the maximum amount of retries */
#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT      BIT1

static const char *TAG_WIFI = "wifi station";

static int s_retry_num = 0;


wifi_config_t wifi_config = {
    .sta = {
        .ssid = {'N', 'E', 'T', 'I', 'A', 'S', 'P', 'O', 'T','-', 'h', '6', 'h', 'N', 0},
        .password = {'M', 'p', 'b', 'C', '6', 's', 'e', '7', 'W', 'e', '9', 'P', 'c', 0},
        /* Setting a password implies station will connect to all security modes including WEP/WPA.
            * However these modes are deprecated and not advisable to be used. Incase your Access point
            * doesn't support WPA2, these mode can be enabled by commenting below line */
        .threshold = {.authmode = WIFI_AUTH_WPA2_PSK},
        .pmf_cfg = {
            .capable = true,
            .required = false
        }
    }
};

static void event_handler(void* arg, esp_event_base_t event_base,
                                int32_t event_id, void* event_data);

EventGroupHandle_t wifi_init_sta(void);
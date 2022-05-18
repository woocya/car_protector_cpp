/* Hello World Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "sim_sms.h"
#include "gps_data_parsing.h"
#include "uart_config.h"
#include "car_talking.h"

static void cos(void *arg) {
    uart_config();

    Sim sim(UART_SIM_PORT_NUM);
    GPSParsing gps(UART_SIM_PORT_NUM);
    
    //vTaskDelay(5000 / portTICK_PERIOD_MS);
    // for (int a=0;a<10;a++) {
    //     uart_write_bytes(UART_OBD_PORT_NUM, "before\n", 7);
    //     vTaskDelay(500 / portTICK_PERIOD_MS);
    // }
    //vTaskDelay(1000 / portTICK_PERIOD_MS);
    sim.InitializeSIM();

    gps.ActivateGps();
    gps.GetData();
    gps.ParseData();

    int status = 3;
    while (1) {
        gps.GetData();
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        gps.ParseData();
        status = (int)gps.GetGNSSStatus();
        // char a = (char)gps.buffer[0];
        // char b = (char)gps.buffer[1];
        // uart_write_bytes(UART_OBD_PORT_NUM, "\nstart\n", 7);
        // uart_write_bytes(UART_OBD_PORT_NUM, (char *)gps.buffer, gps.len);
        // uart_write_bytes(UART_OBD_PORT_NUM, "\nend\n", 5);
    }
    // status = (int)gps.GetGNSSStatus();
    // char * ch = (char *) malloc(2);
    // sprintf(ch, "%d", status);
    // uart_write_bytes(UART_OBD_PORT_NUM, "\nthis is the value\n", 19);
    // uart_write_bytes(UART_OBD_PORT_NUM, ch, 1);
    // vTaskDelay(1000 / portTICK_PERIOD_MS);
    // while (1) {}
    // char * command = new char[12];
    // sprintf(command, "%d", status);
    // sim.SendSMS(command);
    //sim.SendSMS("works");
}

extern "C" void app_main(void) // needed for cpp programming
{
    // printf("Hello world!\n");

    // /* Print chip information */
    // esp_chip_info_t chip_info;
    // esp_chip_info(&chip_info);
    // printf("This is ESP32 chip with %d CPU cores, WiFi%s%s, ",
    //         chip_info.cores,
    //         (chip_info.features & CHIP_FEATURE_BT) ? "/BT" : "",
    //         (chip_info.features & CHIP_FEATURE_BLE) ? "/BLE" : "");

    // printf("silicon revision %d, ", chip_info.revision);

    // printf("%dMB %s flash\n", spi_flash_get_chip_size() / (1024 * 1024),
    //         (chip_info.features & CHIP_FEATURE_EMB_FLASH) ? "embedded" : "external");

    // for (int i = 10; i >= 0; i--) {
    //     printf("Restarting in %d seconds...\n", i);
    //     vTaskDelay(1000 / portTICK_PERIOD_MS);
    // }
    // printf("Restarting now.\n");
    // fflush(stdout);
    // esp_restart();
   xTaskCreate(cos, "cos_task", UART_SIM_STACK_SIZE, NULL, 10, NULL);
}



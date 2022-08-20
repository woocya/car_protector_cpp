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
#include "database_talking.h"
#include "mainwifi.h"
#include "wifi/src/http_request.cpp"

Main App;

static void cos(void *arg) {
    //gpio_set_level(GPIO_NUM_15, 0);
    uart_config();
    vTaskDelay(5000 / portTICK_PERIOD_MS);
    uart_write_bytes(UART_OBD_PORT_NUM, "before\n", 7);

    Sim sim(UART_SIM_PORT_NUM);
    CarTalking ct(UART_SIM_PORT_NUM);

    sim.InitializeSIM();
    //sim.SendSMS("Sim initialized.");
    // char a = ct.TurnEchoOff();
    // sim.SendSMS(a);
    if (ct.TurnEchoOff() == true) {
        //sim.SendSMS("Echo turned off.");
    }
    //sim.SendSMS("Echo turned off.");
    if (ct.GetObdStarted() == true) {
        sim.SendSMS("Obd responded correctly.");
    }
}

extern "C" void app_main(void) // needed for cpp programming
{
    xTaskCreate(cos, "cos_task", UART_SIM_STACK_SIZE, NULL, 10, NULL);
}



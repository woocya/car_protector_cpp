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
#include "wifi/src/example_wifi.cpp"

Main App;

static void cos(void *arg) {
    //gpio_set_level(GPIO_NUM_15, 0);
    uart_config();
    vTaskDelay(5000 / portTICK_PERIOD_MS);
    uart_write_bytes(UART_OBD_PORT_NUM, "before\r\n", 8);
    
    // wifi_init_sta();
    // http_rest_with_url();

    // while (true)
    // {
    //     vTaskDelay(pdMS_TO_TICKS(1000));
    //     App.run();
    //     uart_write_bytes(UART_OBD_PORT_NUM, "Another attempt...\r\n", 20);
    //     if (App.Wifi.GetState() == WIFI::Wifi::state_e::CONNECTED) {
    //         uart_write_bytes(UART_OBD_PORT_NUM, "Going to do http request...\r\n", 29);
    //         http_rest_with_url();
    //         vTaskDelay(5000 / portTICK_PERIOD_MS);
    //     }
        
    // }

    Sim sim(UART_SIM_PORT_NUM);
    //CarTalking ct(UART_SIM_PORT_NUM);
    //DatabaseTalking dt(UART_SIM_PORT_NUM);

    sim.InitializeSIM();
    //dt.ActivateGPRS();
    //dt.GetDataFromDatabase();
    // //sim.SendSMS("Sim initialized.");
    // // char a = ct.TurnEchoOff();
    // // sim.SendSMS(a);
    // if (sim.ct.TurnEchoOff() == true) {
    //   uart_write_bytes(UART_OBD_PORT_NUM, "####### works\r\n", 16);
    //     //sim.SendSMS("Echo turned off.");
    // }
    // //sim.SendSMS("Echo turned off.");
    sim.ct.configureBluetooth();

    if (sim.ct.GetObdStarted() == true) {
      uart_write_bytes(UART_OBD_PORT_NUM, "Success!!!\r\n", 12);
      //  sim.SendSMS("Obd responded correctly.");
    }

    sim.ct.SetProtocol();

    float speed = sim.ct.AskEngineSpeed();
    char* ch = (char*) malloc(9 + 327);
    sprintf(ch, "speed: %f\r\n", speed);
    uart_write_bytes(UART_OBD_PORT_NUM, ch, 9 + sizeof(speed));

    sim.ct.AskPids1();
    
    uart_write_bytes(UART_OBD_PORT_NUM, "active pids: ", 13);
    uart_write_bytes(UART_OBD_PORT_NUM, sim.ct.getActivePids(), 4);

    // sim.ct.AskVehicleSpeed();

    // sim.ct.AskRuntime();
    free(ch);
    vTaskDelay(10000 / portTICK_PERIOD_MS);
}

extern "C" void app_main(void) // needed for cpp programming
{
    // esp_err_t ret = nvs_flash_init();
    // if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    //   ESP_ERROR_CHECK(nvs_flash_erase());
    //   ret = nvs_flash_init();
    // }
    // ESP_ERROR_CHECK(ret);

    // ESP_LOGI(TAG_WIFI, "ESP_WIFI_MODE_STA");
    xTaskCreate(cos, "cos_task", UART_SIM_STACK_SIZE, NULL, 10, NULL);
}



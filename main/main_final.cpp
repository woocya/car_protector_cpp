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
#include "example_wifi.h"
#include "values.h"
#include "wifi/src/http_request.cpp"

//Main App;

static void cos(void *arg) {
    //gpio_set_level(GPIO_NUM_15, 0);
    uart_config();

    Sim sim(UART_SIM_PORT_NUM);
    sim.InitializeSIM();

    sim.ct.bt.Configure();

    sim.ct.GetObdStarted();
    sim.ct.SetProtocol();

    wifi_init_sta();
    
    //CarTalking ct(UART_SIM_PORT_NUM);
    DatabaseTalking dt(UART_SIM_PORT_NUM);
    dt.ActivateGPRS();

    EventBits_t bits;

    Values values;
    char * limits_buffer;

    while(1) {

        bits = xEventGroupWaitBits(s_wifi_event_group,
                WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
                pdFALSE,
                pdFALSE,
                portMAX_DELAY);

        if (bits & WIFI_CONNECTED_BIT) {
            limits_buffer = http_rest_with_url();
        }
        else if (bits & WIFI_FAIL_BIT) {
            limits_buffer = dt.GetDataFromDatabase();
        }
        
        if (sim.ct.GetObdStarted() != true) {
            esp_restart();
        }
        
        values.setActive(sim.ct.AskEngineSpeed());

        // sim.ct.AskVehicleSpeed();

        // sim.ct.AskRuntime();

        vTaskDelay(10000 / portTICK_PERIOD_MS);
    }

}

// extern "C" void app_main(void) // needed for cpp programming
// {
//     // esp_err_t ret = nvs_flash_init();
//     // if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
//     //   ESP_ERROR_CHECK(nvs_flash_erase());
//     //   ret = nvs_flash_init();
//     // }
//     // ESP_ERROR_CHECK(ret);

//     // ESP_LOGI(TAG_WIFI, "ESP_WIFI_MODE_STA");
//     xTaskCreate(cos, "cos_task", UART_SIM_STACK_SIZE, NULL, 10, NULL);
// }



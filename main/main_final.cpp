#include "car_talking.h"
#include "config_wifi.h"
#include "database_talking.h"
#include "gps_data_parsing.h"
#include "http_request.h"
#include "motion_sensor.h"
#include "sim_sms.h"
#include "uart_config.h"
#include "values.h"

#include "esp_spi_flash.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include <stdio.h>

static void cos(void *arg) {
    //gpio_set_level(GPIO_NUM_15, 0);
    uart_config();

    Sim sim(UART_SIM_PORT_NUM);
    sim.initializeSIM();

    sim.ct.bt.configure();

    sim.ct.getObdStarted();
    sim.ct.setProtocol();

    wifi_init_sta();
    
    //CarTalking ct(UART_SIM_PORT_NUM);
    DatabaseTalking dt(UART_SIM_PORT_NUM);
    dt.activateGPRS();

    GPSParsing gps(UART_SIM_PORT_NUM);

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
            limits_buffer = http_rest_with_url(0, nullptr);
        }
        else if (bits & WIFI_FAIL_BIT) {
            limits_buffer = dt.getDataFromDatabase();
        }

        values.parse(limits_buffer);
        
        if (sim.ct.getObdStarted() != true) {
            esp_restart();
        }

        sim.ct.checkAvailableParams();
        
        unsigned char* pids;
        pids = sim.ct.getActivePids();
        for (int num_of_param = 0; num_of_param < 4; num_of_param++) {
            if (pids[num_of_param] & 1) {
                switch(num_of_param) {
                    case 0:
                        values.setActive(sim.ct.askEngineSpeed());
                        break;
                    case 1:
                        values.setCarSpeed(sim.ct.askVehicleSpeed());
                        break;
                    case 2:
                        values.setFuelLevel(sim.ct.askFuelLevel());
                        break;
                    case 3:
                        values.setRuntime(sim.ct.askRuntime());
                        break;
                    default:
                        break;
                }
            } 
        }

        values.setMotionSensor();

        gps.activateGps();
        gps.getData();
        gps.parseData();

        values.setDate(gps.getYear(), gps.getMonth(), gps.getDay());
        values.setTime(gps.getHour(), gps.getMinute());
        values.setLatitude(gps.getLatitude());
        values.setLongitude(gps.getLongitude());

        if (values.getRuntime() != -1) {
            values.countTimeWithRuntime();
        }

        values.compareAndWarn(sim);
        

        if (bits & WIFI_CONNECTED_BIT) {
            http_rest_with_url(1, values.constructPostMessage());
        }
        else if (bits & WIFI_FAIL_BIT) {
            //dt.SendDataToDatabase(values.constructPostMessage());
        }

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
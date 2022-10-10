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
#include "values.h"
#include "mainwifi.h"
#include "wifi/src/http_request.cpp"
#include "wifi/src/example_wifi.cpp"

Main App;

static void cos(void *arg) {
    //gpio_set_level(GPIO_NUM_15, 0);
    uart_config();
    vTaskDelay(5000 / portTICK_PERIOD_MS);
    uart_write_bytes(UART_OBD_PORT_NUM, "before\r\n", 8);

    Sim sim(UART_SIM_PORT_NUM);
    sim.InitializeSIM();


    DatabaseTalking dt(UART_SIM_PORT_NUM);
    dt.ActivateGPRS();

    GPSParsing gps(UART_SIM_PORT_NUM);

    Values values;
    unable_to_connect = 0;
    
    uart_write_bytes(UART_OBD_PORT_NUM, "stuff initialized\r\n", 19);

    //wifi_init_sta();
    EventBits_t bits;
    char * limits_buffer = (char*)malloc(100);
    bits = xEventGroupWaitBits(wifi_init_sta(),
                WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
                pdFALSE,
                pdFALSE,
                portMAX_DELAY);

    
    uart_write_bytes(UART_OBD_PORT_NUM, "wifi figured out\r\n", 18);

    if (bits & WIFI_CONNECTED_BIT) {
        uart_write_bytes(UART_OBD_PORT_NUM, "wifi\r\n", 6);
        limits_buffer = http_rest_with_url(0, "http://car-protector.herokuapp.com/getLimits");
    }
    else if (bits & WIFI_FAIL_BIT) {
         uart_write_bytes(UART_OBD_PORT_NUM, "gprs\r\n", 6);
         limits_buffer = dt.GetDataFromDatabase();
    }

    values.parse(limits_buffer);
    free(limits_buffer);
    uart_write_bytes(UART_OBD_PORT_NUM, "parsed\r\n", 8);
    

    sim.ct.configureBluetooth();
    if (sim.ct.GetObdStarted()) {
        sim.ct.SetProtocol();

        // sim.ct.CheckAvailableParams();
        
        // unsigned char* pids;
        // pids = sim.ct.getActivePids();
        // for (int num_of_param = 0; num_of_param < 4; num_of_param++) {
        //     if (pids[num_of_param] & 1) {
        //         switch(num_of_param) {
        //             case 0:
                           values.setActive(sim.ct.AskEngineSpeed());
        //                 sim.ct.AskEngineSpeed();
                           uart_write_bytes(UART_OBD_PORT_NUM, "ENGINE\r\n", 8);
        //                 break;
        //             case 1:
                           values.setCarSpeed(sim.ct.AskVehicleSpeed());
        //                 sim.ct.AskVehicleSpeed();
                           uart_write_bytes(UART_OBD_PORT_NUM, "VEHICLE\r\n", 9);
        //                 break;
        //             case 2:
                           values.setFuelLevel(sim.ct.AskFuelLevel());
        //                 sim.ct.AskFuelLevel();
                           uart_write_bytes(UART_OBD_PORT_NUM, "FUEL\r\n", 6);
        //                 break;
        //             case 3:
        //                   values.setRuntime(sim.ct.AskRuntime());
        //                 sim.ct.AskRuntime();
        //                   uart_write_bytes(UART_OBD_PORT_NUM, "RUNTIME\r\n", 9);
        //                 break;
        //             default:
        //                 break;
        //         }
        //     } 
      //  }
    }

    uart_write_bytes(UART_OBD_PORT_NUM, "gps start\r\n", 11);
    gps.ActivateGps();

    while (gps.GetFixStatus() != 1) {
        gps.GetData();
        gps.ParseData();
    }
    uart_write_bytes(UART_OBD_PORT_NUM, "got gps data\r\n", 14);

    values.setRuntime(sim.ct.AskRuntime());
    uart_write_bytes(UART_OBD_PORT_NUM, "RUNTIME\r\n", 9);
    

    values.setDate(gps.GetYear(), gps.GetMonth(), gps.GetDay());
    values.setTime(gps.GetHour(), gps.GetMinute());
    values.setLatitude(gps.GetLatitude());
    values.setLongitude(gps.GetLongitude());
    // // values.setActive(true);    
    // // values.setCarSpeed(50);
    // // values.setFuelLevel(50.0);
    // values.setLatitude(gps.GetLatitude());
    // values.setLongitude(gps.GetLongitude());
     values.setMotionSensor(false);
     values.setDateOfCar(2022, 9, 19);
     values.setTimeOfCar(23, 23);

    uart_write_bytes(UART_OBD_PORT_NUM, "values set\r\n", 12);

    //values.compareAndWarn(sim);
    uart_write_bytes(UART_OBD_PORT_NUM, "warning sent\r\n", 14);

    //dt.SendDataToDatabase(values.constructPostMessage());

    if (bits & WIFI_CONNECTED_BIT) {
        uart_write_bytes(UART_OBD_PORT_NUM, "wifi\r\n", 6);
        http_rest_with_url(1, values.constructPostMessage());
    }
    else if (bits & WIFI_FAIL_BIT) {
         uart_write_bytes(UART_OBD_PORT_NUM, "gprs\r\n", 6);
        dt.SendDataToDatabase(values.constructPostMessage());
    }
    else {
        uart_write_bytes(UART_OBD_PORT_NUM, "nothing?\r\n", 10);
    }

    // sim.InitializeSIM();
    // //dt.ActivateGPRS();
    // //dt.GetDataFromDatabase();
    // // //sim.SendSMS("Sim initialized.");
    // // // char a = ct.TurnEchoOff();
    // // // sim.SendSMS(a);
    // // if (sim.ct.TurnEchoOff() == true) {
    // //   uart_write_bytes(UART_OBD_PORT_NUM, "####### works\r\n", 16);
    // //     //sim.SendSMS("Echo turned off.");
    // // }
    // // //sim.SendSMS("Echo turned off.");
    // sim.ct.configureBluetooth();

    // if (sim.ct.GetObdStarted() == true) {
    //   uart_write_bytes(UART_OBD_PORT_NUM, "Success!!!\r\n", 12);
    //   //  sim.SendSMS("Obd responded correctly.");
    // }

    // sim.ct.SetProtocol();

    // float speed = sim.ct.AskFuelLevel();
    // char* ch = (char*) malloc(9 + 327);
    // sprintf(ch, "level: %f\r\n", speed);
    // uart_write_bytes(UART_OBD_PORT_NUM, ch, 9 + sizeof(speed));

    // sim.ct.AskPids1();
    
    // uart_write_bytes(UART_OBD_PORT_NUM, "active pids: ", 13);
    // uart_write_bytes(UART_OBD_PORT_NUM, sim.ct.getActivePids(), 4);

    // // sim.ct.AskVehicleSpeed();

    // // sim.ct.AskRuntime();
    // free(ch);
    vTaskDelay(10000 / portTICK_PERIOD_MS);
}

extern "C" void app_main(void) // needed for cpp programming
{
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    ESP_LOGI(TAG_WIFI, "ESP_WIFI_MODE_STA");
    xTaskCreate(cos, "cos_task", UART_SIM_STACK_SIZE, NULL, 10, NULL);
}



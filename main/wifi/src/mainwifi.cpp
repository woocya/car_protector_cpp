#include "mainwifi.h"
#include "uart_talk.h"

//Main App;

void Main::run(void)
{
    wifiState = Wifi.GetState();

    switch (wifiState)
    {
    case WIFI::Wifi::state_e::READY_TO_CONNECT:
        uart_write_bytes(UART_OBD_PORT_NUM, "Wifi Status: READY_TO_CONNECT\n", 31);
        std::cout << "Wifi Status: READY_TO_CONNECT\n";
        Wifi.Begin();
        break;
    case WIFI::Wifi::state_e::DISCONNECTED:
        uart_write_bytes(UART_OBD_PORT_NUM, "Wifi Status: DISCONNECTED\n", 27);
        std::cout << "Wifi Status: cccc\n";
        Wifi.Begin();
        break;
    case WIFI::Wifi::state_e::CONNECTING:
        uart_write_bytes(UART_OBD_PORT_NUM, "Wifi Status: CONNECTING\n", 25);
        std::cout << "Wifi Status: CONNECTING\n";
        break;
    case WIFI::Wifi::state_e::WAITING_FOR_IP:
        uart_write_bytes(UART_OBD_PORT_NUM, "Wifi Status: WAITING_FOR_IP\n", 29);
        std::cout << "Wifi Status: WAITING_FOR_IP\n";
        break;
    case WIFI::Wifi::state_e::ERROR:
        uart_write_bytes(UART_OBD_PORT_NUM, "Wifi Status: ERROR\n", 20);
        std::cout << "Wifi Status: ERROR\n";
        break;
    case WIFI::Wifi::state_e::CONNECTED:
        uart_write_bytes(UART_OBD_PORT_NUM, "Wifi Status: CONNECTED\n", 24);
        std::cout << "Wifi Status: CONNECTED\n";
        break;
    case WIFI::Wifi::state_e::NOT_INITIALIZED:
        uart_write_bytes(UART_OBD_PORT_NUM, "Wifi Status: NOT_INITIALIZED\n", 30);
        std::cout << "Wifi Status: NOT_INITIALIZED\n";
        break;
    case WIFI::Wifi::state_e::INITIALIZED:
        uart_write_bytes(UART_OBD_PORT_NUM, "Wifi Status: INITIALIZED\n", 26);
        std::cout << "Wifi Status: INITIALIZED\n";
        break;
    }
}

void Main::setup(void)
{
    esp_event_loop_create_default();
    //nvs_flash_init();

    Wifi.SetCredentials("toya1234568000895", "32636501");
    Wifi.Init();
}

// extern "C" void app_main(void)
// {
//     App.setup();
//     while (true)
//     {
//         vTaskDelay(pdMS_TO_TICKS(1000));
//         App.run();
//     }
// }
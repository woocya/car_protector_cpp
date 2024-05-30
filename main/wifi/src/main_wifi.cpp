#include "main_wifi.h"
#include "uart_talk.h"

void Main::run(void)
{
    wifiState = Wifi.GetState();

    switch (wifiState)
    {
    case WIFI::Wifi::state_e::READY_TO_CONNECT:
        uart_write_bytes(UART_OBD_PORT_NUM, "wifi Status: READY_TO_CONNECT\n", 31);
        std::cout << "wifi Status: READY_TO_CONNECT\n";
        Wifi.Begin();
        break;
    case WIFI::Wifi::state_e::DISCONNECTED:
        uart_write_bytes(UART_OBD_PORT_NUM, "wifi Status: DISCONNECTED\n", 27);
        std::cout << "wifi Status: cccc\n";
        Wifi.Begin();
        break;
    case WIFI::Wifi::state_e::CONNECTING:
        uart_write_bytes(UART_OBD_PORT_NUM, "wifi Status: CONNECTING\n", 25);
        std::cout << "wifi Status: CONNECTING\n";
        break;
    case WIFI::Wifi::state_e::WAITING_FOR_IP:
        uart_write_bytes(UART_OBD_PORT_NUM, "wifi Status: WAITING_FOR_IP\n", 29);
        std::cout << "wifi Status: WAITING_FOR_IP\n";
        break;
    case WIFI::Wifi::state_e::ERROR:
        uart_write_bytes(UART_OBD_PORT_NUM, "wifi Status: ERROR\n", 20);
        std::cout << "wifi Status: ERROR\n";
        break;
    case WIFI::Wifi::state_e::CONNECTED:
        uart_write_bytes(UART_OBD_PORT_NUM, "wifi Status: CONNECTED\n", 24);
        std::cout << "wifi Status: CONNECTED\n";
        break;
    case WIFI::Wifi::state_e::NOT_INITIALIZED:
        uart_write_bytes(UART_OBD_PORT_NUM, "wifi Status: NOT_INITIALIZED\n", 30);
        std::cout << "wifi Status: NOT_INITIALIZED\n";
        break;
    case WIFI::Wifi::state_e::INITIALIZED:
        uart_write_bytes(UART_OBD_PORT_NUM, "wifi Status: INITIALIZED\n", 26);
        std::cout << "wifi Status: INITIALIZED\n";
        break;
    }
}

void Main::setup(void)
{
    esp_event_loop_create_default();

    Wifi.SetCredentials("toya1234568000895", "32636501");
    Wifi.Init();
}
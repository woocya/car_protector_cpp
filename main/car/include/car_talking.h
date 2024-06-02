// 1. atz (odpowiedz sugeruje ze polaczenie modulu jest ok)
// 2. atsp2 lub atsp3 (ustawienie odpowiedniego protokolu)
#ifndef CAR_TALKING_H_
#define CAR_TALKING_H_

#include "bluetooth_config.h"
#include "uart_talk.h"

#include "driver/gpio.h"
#include "driver/uart.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sdkconfig.h"

#include <cstring>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <vector>

class CarTalking: UartTalk {
protected:
    static unsigned char checked_pids[4]; 
    static unsigned char active_pids[4];
public:
    CarTalking(int where): UartTalk(where), bt(where) {}

    void configureBluetooth();
    bool getObdStarted();
    bool setProtocol();
    void checkAvailableParams();
    void checkPidsSupported(uint8_t* command_set, int size); // check if given vehicle support necessary pids
    unsigned char* getActivePids();
    void readAndProcessMessage(const char* command_to_send, int len_of_command, char * buffer_to_read, int size_of_read_buffer, int wait_for, bool skip_info = false); // read data with uart
    float askEngineSpeed();
    float askFuelLevel();
    bool askPids1();
    bool askPids2();
    int askRuntime();
    int askVehicleSpeed();

    BluetoothConfig bt;
};

#endif // CAR_TALKING_H_
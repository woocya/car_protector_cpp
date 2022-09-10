// 1. atz (odpowiedz sugeruje ze polaczenie modulu jest ok)
// 2. atsp2 lub atsp3 (ustawienie odpowiedniego protokolu)
// 3. 
#ifndef CAR_TALKING_H_
#define CAR_TALKING_H_

#include <vector>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#include "sdkconfig.h"
#include "esp_log.h"

#include "bluetooth_config.h"
#include "uart_talk.h"

class CarTalking: UartTalk {
protected:
    static unsigned char checked_pids[4]; 
    static unsigned char active_pids[4]; 
    uint8_t is_prompt_char;
public:
    CarTalking(int where): UartTalk(where), bt(where) {
  
        int a = bt.Configure();
    }

    BluetoothConfig bt;
    
    void ReadAndProcessMessage(const char* command_to_send, int len_of_command, char * buffer_to_read, int size_of_read_buffer, int wait_for, bool skip_info = false); // read data with uart

    bool GetObdStarted();

    bool TurnEchoOff();

    bool SetProtocol();

    void CheckPidsSupported(uint8_t* command_set, int size); // check if given vehicle support necessary pids

    bool AskPids1();
    bool AskPids2();
    float AskEngineSpeed();
    int AskVehicleSpeed();
    float AskRuntime();
    float AskFuelLevel();

    void compareLimits();
};

#endif // CAR_TALKING_H_
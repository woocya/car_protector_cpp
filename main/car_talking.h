// 1. atz (odpowiedz sugeruje ze polaczenie modulu jest ok)
// 2. atsp2 lub atsp3 (ustawienie odpowiedniego protokolu)
// 3. 
#ifndef CAR_TALKING_H_
#define CAR_TALKING_H_

#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#include "sdkconfig.h"
#include "esp_log.h"

#include "uart_talk.h"

class CarTalking: UartTalk {
protected:
    static unsigned char checked_pids[4]; 
    static unsigned char active_pids[4]; 
    uint8_t is_prompt_char;
public:
    CarTalking(int where);
    bool GetObdStarted();

    bool TurnEchoOff();

    bool SetProtocol();

    void CheckPidsSupported(uint8_t* command_set, int size); // check if given vehicle support necessary pids

    int ReadAndProcessMessage(const char* command_to_send, int expected_bytes); // read data with uart

    bool AskPids1();
    bool AskPids2();
    float AskEngineSpeed();
    float AskVehicleSpeed();
    float AskRuntime();
    float AskFuelLevel();
};

unsigned char CarTalking::checked_pids[4] = {0b0000'0001, 0b0000'0010, 0b0000'0100, 0b0000'1000}; // engine_speed_active, vehicle_speed_active, runtime_active, fuel_tank_level_active
unsigned char CarTalking::active_pids[4] = {0b0000'0000, 0b0000'0000, 0b0000'0000, 0b0000'0000};

#endif // CAR_TALKING_H_
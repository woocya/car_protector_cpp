// 1. atz (odpowiedz sugeruje ze polaczenie modulu jest ok)
// 2. atsp2 lub atsp3 (ustawienie odpowiedniego protokolu)
// 3. 
#ifndef CAR_TALKING_H_
#define CAR_TALKING_H_

#include <vector>
#include <stdio.h>
#include <cstring>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#include "sdkconfig.h"
#include "esp_log.h"

#include "pid.h"

#define UART_TXD_PIN_NUM (1)
#define UART_RXD_PIN_NUM (3)
#define UART_RTS_PIN_NUM (UART_PIN_NO_CHANGE)
#define UART_CTS_PIN_NUM (UART_PIN_NO_CHANGE)

#define UART_PORT_NUM      (UART_NUM_1)
#define UART_BAUD_RATE     (38400)
//#define ECHO_TASK_STACK_SIZE    (CONFIG_EXAMPLE_TASK_STACK_SIZE)
#define IN_BUF_SIZE (14) // max for response is 12 + "\r" + ">"
#define OUT_BUF_SIZE (4)

class CarTalking {
protected:
    uint8_t* data_from_car;
    uint8_t* data_for_car;
public:
    int CountBytes();

    bool TalkToObd(PId command_to_send); //send commands to and receive data from obd

    void CheckPidsSupported(uint8_t* command_set, int size); // check if given vehicle support necessary pids

    int GetCarInfo(PId command_to_send); // obtain desired data from a vehicle

    float TranslateInfo(PId sent_command); // convert received from a vehicle data into information

    bool UartConfig(); // configure uart connection

    bool UartRead(int expected_bytes); // read data with uart

    bool UartWrite(PId pid, int bytes_from_last_response); // write commands with uart
};

#endif CAR_TALKING_H_
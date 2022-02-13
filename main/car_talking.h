// 1. atz (odpowiedz sugeruje ze polaczenie modulu jest ok)
// 2. atsp2 lub atsp3 (ustawienie odpowiedniego protokolu)
// 3. 

#include <vector>
#include <stdio.h>
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
#define BUF_SIZE (1024)

class CarTalking {
protected:
    uint8_t* dataFromCar;
    uint8_t* dataForCar;
public:
    CarTalking();

    bool checkConnection();

    bool setProtocol(); //atsp2 lub atsp3   

    int getInfo(PId command, int expected_bits); 

    int translateInfo(); 

    bool uartConfig();

    void uartRead();

    void uartWrite(PId pid, char whatToWrite);
};


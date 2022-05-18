#ifndef UART_CONFIG_H_
#define UART_CONFIG_H_

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#include "sdkconfig.h"

#define UART_SIM_TXD (5)
#define UART_SIM_RXD (4)
#define UART_OBD_TXD (1)
#define UART_OBD_RXD (3)
#define UART_RTS (UART_PIN_NO_CHANGE)
#define UART_CTS (UART_PIN_NO_CHANGE)

#define UART_SIM_PORT_NUM      (UART_NUM_0)
#define UART_OBD_PORT_NUM      (UART_NUM_1)
#define UART_SIM_BAUD_RATE     (115200)
#define UART_OBD_BAUD_RATE     (38400)
#define UART_SIM_STACK_SIZE    (4096)

#define UART_BUF_SIZE (1024)


void uart_config();

#endif // UART_CONFIG_H_
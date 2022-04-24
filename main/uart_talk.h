#ifndef UART_TALK_H_
#define UART_TALK_H_

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"
#include <memory>
#include <string.h>
#include <string>

#include "uart_config.h"

class UartTalk {
protected:
    int where;
    uint8_t* buffer;
public:
    UartTalk(int where);
    ~UartTalk();
    int UartConversation(const char *command, int wait_for);
};


#endif UART_TALK_H_
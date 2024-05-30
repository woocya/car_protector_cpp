#ifndef UART_TALK_H_
#define UART_TALK_H_

#include "uart_config.h"

#include "driver/uart.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include <string.h>

extern bool status;

class UartTalk {
protected:    
    int where;
public:
    uint8_t* buffer;
    UartTalk(int where);
    ~UartTalk();
    int UartConversation(const char *command, int wait_for);
};


#endif // UART_TALK_H_
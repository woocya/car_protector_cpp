#ifndef SIM_SMS_H_
#define SIM_SMS_H_

#include <stdio.h>
#include <cstring>
#include <string>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#include "sdkconfig.h"

#include "uart_talk.h"

class Sim: public UartTalk {
protected:
    char* server_name;
public:
    Sim(int where): UartTalk(where) {}
    bool InitializeSIM();
    bool InitializeGPRS();
    bool SendSMS(const char * what_to_send);
    void SendPackage(int length, const char* what_to_send);
    // int ReceivePackage();
};




#endif // SIM_SMS_H_
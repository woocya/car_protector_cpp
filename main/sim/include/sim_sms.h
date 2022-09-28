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
#include "car_talking.h"

class Sim: public UartTalk {
protected:
    char* server_name;
    char telephone_number[9];
public:
    CarTalking ct;
    Sim(int where): UartTalk(where), ct(where) {}

    void SetTelephoneNumber(char telephone_number[9]);
    bool InitializeSIM();
    bool SendSMS(const char * what_to_send);
};




#endif // SIM_SMS_H_
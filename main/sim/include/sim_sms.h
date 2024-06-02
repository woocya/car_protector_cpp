#ifndef SIM_SMS_H_
#define SIM_SMS_H_

#include "car_talking.h"
#include "uart_talk.h"

#include "driver/gpio.h"
#include "driver/uart.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sdkconfig.h"

#include <cstring>
#include <stdio.h>
#include <string>

class Sim: public UartTalk {
protected:
    char* server_name;
    char telephone_number[9];
public:
    Sim(int where): UartTalk(where), ct(where) {}

    bool initializeSIM();
    bool sendSMS(const char * what_to_send);
    void setTelephoneNumber(char telephone_number[9]);

    CarTalking ct;
};

#endif // SIM_SMS_H_
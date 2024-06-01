#ifndef GPS_DATA_PARSING_H_
#define GPS_DATA_PARSING_H_

#include "uart_talk.h"

#include "driver/gpio.h"
#include "driver/uart.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include <cstring>
#include <map>
#include <stdio.h>

class GPSParsing: public UartTalk {
private:
    std::map<std::string, float> gps_data = {{"GNSS run status", 2}, {"Fix status", 0}, {"year", 0}, {"month", 0}, {"day", 0}, {"hour", 0}, {"minute", 0}, {"second", 0}, {"latitude", 0}, {"longitude", 0}, {"MLS altitude", 0}, {"speed", 0}};
public:
    GPSParsing(int where);
    
    void activateGps();
    void getData();
    void parseData();
    int getGNSSStatus();
    int getFixStatus();
    int getYear();
    int getMonth();
    int getDay();
    int getHour();
    int getMinute();
    int getSecond();
    float getLatitude();
    float getLongitude();
    float getAltitude();
    float getSpeed();
    void setFixStatus(int status);

    int len;
};


#endif // GPS_DATA_PARSING_H_
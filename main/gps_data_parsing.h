#ifndef GPS_DATA_PARSING_H_
#define GPS_DATA_PARSING_H_

#include <vector>
#include <stdio.h>
#include <cstring>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"
#include "driver/gpio.h"


int activate_gps();
int get_data(int seconds);
int parse_data(enum data);



#endif GPS_DATA_PARSING_H_
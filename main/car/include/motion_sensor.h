#ifndef MOTION_SENSOR_H_
#define MOTION_SENSOR_H_

#include <stdio.h>
#include <cstring>
#include <string>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#include "sdkconfig.h"

bool detect_motion();

#endif MOTION_SENSOR_H_
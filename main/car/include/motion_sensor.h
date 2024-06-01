#ifndef MOTION_SENSOR_H_
#define MOTION_SENSOR_H_

#include "driver/gpio.h"
#include "driver/uart.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sdkconfig.h"

#include <cstring>
#include <stdio.h>
#include <string>

bool detect_motion();

#endif // MOTION_SENSOR_H_
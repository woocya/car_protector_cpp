#include "motion_sensor.h"

bool detect_motion() {
    /* Set the GPIO as a push/pull output */
    int level_first = 2;
    int level_second = 2;
    gpio_set_direction(GPIO_NUM_1, GPIO_MODE_INPUT);
    level_first = gpio_get_level(GPIO_NUM_1);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    level_second = gpio_get_level(GPIO_NUM_1);
    if (level_first & level_second) {
        return true;
    }

    return false;
}

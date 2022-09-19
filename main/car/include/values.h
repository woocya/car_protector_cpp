#ifndef VALUES_H_
#define VALUES_H_

#include <stdio.h>
#include "sim_sms.h"
#include "motion_sensor.h"

struct Time {
    int hour;
    int minute;
};

struct Date {
    int year;
    int month;
    int day;
};

class Values {
protected:
    Time time_from_GPS;
    Time time_of_car_start;
    Date date_from_GPS;
    Date date_of_car_start;
    bool is_active;
    int car_speed;
    float fuel_level;
    int runtime;
    double latitude;
    double longitude;
    bool motion_sensor;

    Time limit_time_min;
    Time limit_time_max;
    double limit_latitude_min;
    double limit_latitude_max;
    double limit_longitude_min;
    double limit_longitude_max;
    bool limit_motion_sensor;
    char telephone_number[9];

public:
    Values() {
        setFuelLevel(-1.0);
        setRuntime(-1);
    }

    void setTime(int hour, int minute);

    void setDate(int year, int month, int day);

    void setActive(bool is_active);
    
    void setCarSpeed(int speed);

    void setFuelLevel(float level);

    void setRuntime(int runtime);

    int getRuntime();

    void countTimeWithRuntime();

    void setLatitude(double latitude);

    void setLongitude(double longitude);

    void setMotionSensor();

    int compareTime(Time a, Time b);

    void compareAndWarn(Sim &sim);

    void parse(const char* buffer);

    const char * constructPostMessage();
};

#endif VALUES_H_
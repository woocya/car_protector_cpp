#ifndef VALUES_H_
#define VALUES_H_

#include "motion_sensor.h"
#include "sim_sms.h"

#include <stdio.h>

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
    int car_speed;
    Date date_of_car_start;
    float fuel_level;
    bool is_active;
    int runtime;
    Time time_of_car_start;

    Date date_from_GPS;
    double latitude;
    double longitude;
    Time time_from_GPS;

    bool motion_sensor;

    double limit_latitude_max;
    double limit_latitude_min;
    double limit_longitude_max;
    double limit_longitude_min;
    bool limit_motion_sensor;
    Time limit_time_min;
    Time limit_time_max;

    char telephone_number[9];

public:
    Values() {
        setFuelLevel(-1.0);
        setRuntime(-1);
    }

    float getFuelLevel();
    int getRuntime();
    
    void setActive(bool is_active);
    void setCarSpeed(int speed);
    void setDate(int year, int month, int day);
    void setFuelLevel(float level);
    void setLatitude(double latitude);
    void setLongitude(double longitude);
    void setMotionSensor();
    void setRuntime(int runtime);
    void setTime(int hour, int minute);
    
    void compareAndWarn(Sim &sim);
    int compareTime(Time a, Time b);
    void countTimeWithRuntime();
    void parse(const char* buffer);

    void setDateOfCar(int year, int month, int day);
    void setMotionSensor(bool is_motion);
    void setTimeOfCar(int hour, int minute);

    char * constructPostMessage();
};

#endif // VALUES_H_
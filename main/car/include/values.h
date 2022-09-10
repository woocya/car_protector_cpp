#include <stdio.h>
#include "sim_sms.h"

struct Time {
    int hour;
    int minute;
};

class Values {
protected:
    Time time;
    bool is_active;
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
    Values() {}

    void setTime(int hour, int minute);

    void setActive(bool is_active);

    void setLatitude(double latitude);

    void setLongitude(double longitude);

    void setMotionSensor(bool motion_sensor);

    int compareTime(Time a, Time b);

    void compareAndWarn(Sim &sim);

    void parse(const char* buffer);
};
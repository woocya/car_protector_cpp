#include "values.h"

void Values::setTime(int hour, int minute) {
    time.hour = hour;
    time.minute = minute;
}

void Values::setActive(bool is_active) {
    this->is_active = is_active;
}

void Values::setLatitude(double latitude) {
    this->latitude = latitude;
}

void Values::setLongitude(double longitude) {
    this->longitude = longitude;
}

void Values::setMotionSensor(bool motion_sensor) {
    this->motion_sensor = motion_sensor;
}

int Values::compareTime(Time a, Time b) {
    if (a.hour != b.hour) {
        if (a.hour > b.hour)
            return 1;
        else
            return -1;
    }
    else {
        if (a.minute != b.minute) {
            if (a.minute > b.minute)
                return 1;
            else
                return -1;
        }
        else
            return 0;
    }
}

void Values::compareAndWarn(Sim &sim) {
    sim.setTelephoneNumber(telephone_number);
    if (compareTime(time, limit_time_min) == 1 || compareTime(time, limit_time_min) == -1) {
        sim.SendSMS("Uwaga! Pojazd aktywny w niedozwolonym czasie!");
    }

    if (latitude < limit_latitude_min || latitude > limit_latitude_max || longitude < limit_longitude_min || longitude > limit_longitude_max) {
        char * message;
        sprintf(message, "Uwaga! Pojazd przekroczył dopuszczalną strefę! Współrzędne to %lf %lf", latitude, longitude);
        sim.SendSMS(message);
    }

    if (motion_sensor && !limit_motion_sensor) {
        sim.SendSMS("Uwaga! Wykryto ruch przy pojeździe!");
    }
}

void Values::parse(const char* buffer) {
    int i = 0;
    int x = 0;
    int values_detected = 0;

    std::string helper_string;
    std::string::size_type sz;

    while (buffer[i] != 0) {
        if (buffer[i] == '=') {
            switch (values_detected) {
                case 0:
                    limit_time_min.hour = buffer[i] - '0';
                    limit_time_min.hour = limit_time_min.hour * 10 + (buffer[i+1] - '0');
                    limit_time_min.minute = buffer[i+3] - '0';
                    limit_time_min.minute = limit_time_min.minute * 10 + (buffer[i+4] - '0');
                    values_detected++;
                    i+=5;
                    break;
                case 1:
                    limit_time_max.hour = buffer[i] - '0';
                    limit_time_max.hour = limit_time_max.hour * 10 + (buffer[i+1] - '0');
                    limit_time_max.minute = buffer[i+3] - '0';
                    limit_time_max.minute = limit_time_max.minute * 10 + (buffer[i+4] - '0');
                    values_detected++;
                    i+=5;
                    break;
                case 2:
                    x = 0;
                    helper_string = buffer[x];
                    x++;
                    while(buffer[x] != ',') {
                        helper_string += buffer[x];
                        x++;
                    }
                    limit_latitude_min = std::stof(helper_string, &sz);

                    x+=2;
                    helper_string = buffer[x];
                    x++;
                    while(buffer[x] != ' ') {
                        helper_string += buffer[x];
                        x++;
                    }
                    limit_longitude_min = std::stof(helper_string, &sz);
                    
                    values_detected++;
                    i+=x;
                    break;
                case 3:
                    x = 0;
                    helper_string = buffer[x];
                    x++;
                    while(buffer[x] != ',') {
                        helper_string += buffer[x];
                        x++;
                    }
                    limit_latitude_max = std::stof(helper_string, &sz);

                    x+=2;
                    helper_string = buffer[x];
                    x++;
                    while(buffer[x] != ' ') {
                        helper_string += buffer[x];
                        x++;
                    }
                    limit_longitude_max = std::stof(helper_string, &sz);

                    values_detected++;
                    i+=x;
                    break;
                case 4:
                    limit_motion_sensor = buffer[i];
                    i++;
                    break;
                case 5:
                    for(x = 0; x < 9; x++) {
                        telephone_number[x] = buffer[i+x];
                    }

                    values_detected++;
                    i+=9;
                    break;
                default:
                    break;
            }
        }
        else {
            ++i;
        }
    }
}
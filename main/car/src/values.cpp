#include "values.h"

void Values::setTime(int hour, int minute) {
    time_from_GPS.hour = hour;
    time_from_GPS.minute = minute;
}

void Values::setDate(int year, int month, int day) {
    date_from_GPS.year = year;
    date_from_GPS.month = month;
    date_from_GPS.day = day;
}

void Values::setActive(bool is_active) {
    this->is_active = is_active;
}
    
void Values::setCarSpeed(int speed) {
    car_speed = speed;
}

void Values::setFuelLevel(float level) {
    fuel_level = level;
}

void Values::setRuntime(int runtime) {
    this->runtime = runtime;
}

int Values::getRuntime() {
    return runtime;
}

void Values::countTimeWithRuntime() {
    int hours = runtime % 3600;
    int minutes = (runtime - hours * 3600) % 60;
    int subtract_hour = 0;

    if (minutes > time_from_GPS.minute) {
        time_of_car_start.hour = time_from_GPS.hour - 1;
        time_of_car_start.minute = 60 - (minutes - time_from_GPS.minute);
        subtract_hour = 1;
    }
    else {
        time_of_car_start.minute = minutes - time_from_GPS.minute;
    }

    if (hours > time_from_GPS.hour) { //################################# implement handling ends of months and years
        date_of_car_start.day = date_from_GPS.day - 1;
        time_of_car_start.hour = 24 - (hours - time_from_GPS.hour - subtract_hour);
    }
    else {
        time_of_car_start.hour = hours - time_from_GPS.hour - subtract_hour;
    }
}

void Values::setLatitude(double latitude) {
    this->latitude = latitude;
}

void Values::setLongitude(double longitude) {
    this->longitude = longitude;
}

void Values::setMotionSensor() {
    motion_sensor = detect_motion();
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
    sim.SetTelephoneNumber(telephone_number);
    if (is_active && (compareTime(time_from_GPS, limit_time_min) == 1 || compareTime(time_from_GPS, limit_time_min) == -1)) {
        sim.SendSMS("Uwaga! Pojazd aktywny w niedozwolonym czasie!");
    }

    if (latitude < limit_latitude_min || latitude > limit_latitude_max || longitude < limit_longitude_min || longitude > limit_longitude_max) {
        char * message;
        sprintf(message, "Uwaga! Pojazd przekroczył dopuszczalną strefę! Współrzędne to %.6lf %.6lf", latitude, longitude);
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

const char * Values::constructPostMessage() {
    std::stringstream message;
    message << "id=1,latitude=" << latitude << ",longitude=" << longitude << ",isActive=" << is_active << ",vehicleSpeed=" << car_speed << ",fuelLevel=" << fuel_level << ",isMotion=" << motion_sensor;
    return message.str().c_str();
}
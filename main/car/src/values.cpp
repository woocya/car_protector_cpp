#include "values.h"

void Values::setTime(int hour, int minute) {
    time_from_GPS.hour = hour + 2;
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
    char * ch =  (char*)malloc(sizeof(int));
    sprintf(ch, "%d", speed);
    uart_write_bytes(UART_OBD_PORT_NUM, ch, 3);
    uart_write_bytes(UART_OBD_PORT_NUM, "\r\n", 2);
}

void Values::setFuelLevel(float level) {
    fuel_level = level;
    char * ch =  (char*)malloc(317);
    sprintf(ch, "%f", level);
    uart_write_bytes(UART_OBD_PORT_NUM, ch, sizeof(float));
    uart_write_bytes(UART_OBD_PORT_NUM, "\r\n", 2);
}

float Values::getFuelLevel() {
    return fuel_level;
}

void Values::setRuntime(int runtime) {
    char * ch =  (char*)malloc(sizeof(int));
    sprintf(ch, "%d", runtime);
    uart_write_bytes(UART_OBD_PORT_NUM, ch, 4);
    uart_write_bytes(UART_OBD_PORT_NUM, "\r\n", 2);
    this->runtime = runtime;
}

int Values::getRuntime() {
    return runtime;
}

void Values::countTimeWithRuntime() {
    int hours = runtime / 3600;
    int minutes = (runtime - hours * 3600) / 60;
    int subtract_hour = 0;

    char* ch = (char*)malloc(5*sizeof(int) + 6);
    sprintf(ch, "%d %d %d %d %d\r\n", time_from_GPS.hour, time_from_GPS.minute, hours, minutes, runtime);
    uart_write_bytes(UART_OBD_PORT_NUM, ch, 25);
    free(ch);
    vTaskDelay(200 / portTICK_PERIOD_MS);

    if (minutes > time_from_GPS.minute) {
        time_of_car_start.hour = time_from_GPS.hour - 1;
        time_of_car_start.minute = 60 - (minutes - time_from_GPS.minute);
        subtract_hour = 1;
    }
    else {
        time_of_car_start.minute = time_from_GPS.minute - minutes;
    }

    if (hours > time_from_GPS.hour) {
        date_of_car_start.day = date_from_GPS.day - 1;
        time_of_car_start.hour = 24 - (hours - time_from_GPS.hour - subtract_hour);
    }
    else {
        time_of_car_start.hour = time_from_GPS.hour - hours - subtract_hour;
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
    sim.setTelephoneNumber(telephone_number);
    if ((compareTime(time_from_GPS, limit_time_min) == -1 || compareTime(time_from_GPS, limit_time_max) == 1)) {
        
        uart_write_bytes(UART_OBD_PORT_NUM, "time surpassed\r\n", 16);
        sim.sendSMS("Uwaga! Pojazd aktywny w niedozwolonym czasie!");
    }

    if (latitude < limit_latitude_min || latitude > limit_latitude_max || longitude < limit_longitude_min || longitude > limit_longitude_max) {
        
        uart_write_bytes(UART_OBD_PORT_NUM, "location surpassed\r\n", 20);
        char * message = (char*)malloc(313);
        sprintf(message, "Uwaga! Pojazd przekroczyl dopuszczalna strefe! Wspolrzedne to %.6lf %.6lf", latitude, longitude);
        sim.sendSMS(message);
        free(message);
    }

    if (motion_sensor && !limit_motion_sensor) {
        
        uart_write_bytes(UART_OBD_PORT_NUM, "motion surpassed\r\n", 18);
        sim.sendSMS("Uwaga! Wykryto ruch przy pojezdzie!");
    }
}

void Values::parse(const char* buffer) {
    int i = 0;
    int x = 0;
    int values_detected = 0;

    std::string helper_string;
    std::string::size_type sz;
    char* ch;

    while (buffer[i] != 0) {
        if (buffer[i] == '=') {
            i++;
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
                    x = i;
                    helper_string = buffer[x];
                    x++;
                    while(buffer[x] != ',') {
                        helper_string += buffer[x];
                        x++;
                    }
                    limit_latitude_min = std::stof(helper_string, &sz);

                    x++;
                    helper_string = buffer[x];
                    x++;
                    while(buffer[x] != ' ') {
                        helper_string += buffer[x];
                        x++;
                    }
                    limit_longitude_min = std::stof(helper_string, &sz);
                    
                    values_detected++;
                    i+=(x-i);
                    
                    break;
                case 3:
                    x = i;
                    helper_string = buffer[x];
                    x++;
                    while(buffer[x] != ',') {
                        helper_string += buffer[x];
                        x++;
                    }
                    limit_latitude_max = std::stof(helper_string, &sz);

                    x++;
                    helper_string = buffer[x];
                    x++;
                    while(buffer[x] != ' ') {
                        helper_string += buffer[x];
                        x++;
                    }
                    limit_longitude_max = std::stof(helper_string, &sz);

                    values_detected++;
                    i+=(x-i);

                    break;
                case 4:
                    limit_motion_sensor = buffer[i];
                    values_detected++;
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

char * Values::constructPostMessage() {
    char message[1190] = {0};
    sprintf(message, "AT+HTTPPARA=URL,\"http://car-protector.herokuapp.com/postData?id=1&latitude=%.6lf&longitude=%.6lf&isActive=%d&dateOfLastStart=%02d-%02d-%d&timeOfLastStart=%d:%d&vehicleSpeed=%d&fuelLevel=%.1f&isMotion=%d\"\r", latitude, longitude, is_active, date_of_car_start.year, date_of_car_start.month, date_of_car_start.day, time_of_car_start.hour, time_of_car_start.minute, car_speed, fuel_level, motion_sensor);

    char * ch = message;
    return ch;
}

void Values::setTimeOfCar(int hour, int minute) {
    time_of_car_start.hour = hour;
    time_of_car_start.minute = minute;
}

void Values::setDateOfCar(int year, int month, int day) {
    date_of_car_start.year = year;
    date_of_car_start.month = month;
    date_of_car_start.day = day;
}
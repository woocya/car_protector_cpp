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
    if (is_active && (compareTime(time_from_GPS, limit_time_min) == -1 || compareTime(time_from_GPS, limit_time_max) == 1)) {
        
        uart_write_bytes(UART_OBD_PORT_NUM, "time surpassed\r\n", 16);
        sim.SendSMS("Uwaga! Pojazd aktywny w niedozwolonym czasie!");
    }

    if (latitude < limit_latitude_min || latitude > limit_latitude_max || longitude < limit_longitude_min || longitude > limit_longitude_max) {
        
        uart_write_bytes(UART_OBD_PORT_NUM, "location surpassed\r\n", 20);
        char * message = (char*)malloc(313);
        sprintf(message, "Uwaga! Pojazd przekroczyl dopuszczalna strefe! Wspolrzedne to %.6lf %.6lf", latitude, longitude);
        sim.SendSMS(message);
        free(message);
    }

    if (motion_sensor && !limit_motion_sensor) {
        
        uart_write_bytes(UART_OBD_PORT_NUM, "motion surpassed\r\n", 18);
        sim.SendSMS("Uwaga! Wykryto ruch przy pojezdzie!");
    }
    sim.SendSMS("Wszystko w porzadku.");
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
                    uart_write_bytes(UART_OBD_PORT_NUM, "case 0\r\n", 8);
                    vTaskDelay(200 / portTICK_PERIOD_MS);
                    limit_time_min.hour = buffer[i] - '0';
                    limit_time_min.hour = limit_time_min.hour * 10 + (buffer[i+1] - '0');
                    limit_time_min.minute = buffer[i+3] - '0';
                    limit_time_min.minute = limit_time_min.minute * 10 + (buffer[i+4] - '0');
                    values_detected++;
                    i+=5;
                    ch = (char*)malloc(2*sizeof(int) + 18);
                    sprintf(ch, "hour: %d, min: %d\r\n", limit_time_min.hour, limit_time_min.minute);
                    uart_write_bytes(UART_OBD_PORT_NUM, ch, 50);
                    free(ch);
                    vTaskDelay(200 / portTICK_PERIOD_MS);
                    break;
                case 1:
                    uart_write_bytes(UART_OBD_PORT_NUM, "case 1\r\n", 8);
                    vTaskDelay(200 / portTICK_PERIOD_MS);
                    limit_time_max.hour = buffer[i] - '0';
                    limit_time_max.hour = limit_time_max.hour * 10 + (buffer[i+1] - '0');
                    limit_time_max.minute = buffer[i+3] - '0';
                    limit_time_max.minute = limit_time_max.minute * 10 + (buffer[i+4] - '0');
                    values_detected++;
                    i+=5;
                    ch = (char*)malloc(2*sizeof(int) + 18);
                    sprintf(ch, "hour: %d, min: %d\r\n", limit_time_max.hour, limit_time_max.minute);
                    uart_write_bytes(UART_OBD_PORT_NUM, ch, 50);
                    vTaskDelay(200 / portTICK_PERIOD_MS);
                    free(ch);
                    break;
                case 2:
                    uart_write_bytes(UART_OBD_PORT_NUM, "case 2\r\n", 8);
                    vTaskDelay(200 / portTICK_PERIOD_MS);
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

                    ch = (char*)malloc(657);
                    sprintf(ch, "min lat: %f, min lon: %f\r\n", limit_latitude_min, limit_longitude_min);
                    uart_write_bytes(UART_OBD_PORT_NUM, ch, 50);
                    free(ch);
                    vTaskDelay(200 / portTICK_PERIOD_MS);
                    
                    break;
                case 3:
                    uart_write_bytes(UART_OBD_PORT_NUM, "case 3\r\n", 8);
                    vTaskDelay(200 / portTICK_PERIOD_MS);
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

                    ch = (char*)malloc(657);
                    sprintf(ch, "min lat: %f, min lon: %f\r\n", limit_latitude_max, limit_longitude_max);
                    uart_write_bytes(UART_OBD_PORT_NUM, ch, 50);
                    free(ch);
                    vTaskDelay(200 / portTICK_PERIOD_MS);

                    break;
                case 4:
                    uart_write_bytes(UART_OBD_PORT_NUM, "case 4\r\n", 8);
                    vTaskDelay(200 / portTICK_PERIOD_MS);
                    limit_motion_sensor = buffer[i];
                    values_detected++;
                    i++;

                    ch = (char*)malloc(sizeof(int) + 15);
                    sprintf(ch, "motion: %d\r\n", limit_motion_sensor);
                    uart_write_bytes(UART_OBD_PORT_NUM, ch, 50);
                    free(ch);
                    vTaskDelay(200 / portTICK_PERIOD_MS);

                    break;
                case 5:
                    uart_write_bytes(UART_OBD_PORT_NUM, "case 5\r\n", 8);
                    vTaskDelay(200 / portTICK_PERIOD_MS);
                    for(x = 0; x < 9; x++) {
                        telephone_number[x] = buffer[i+x];
                    }

                    values_detected++;
                    i+=9;

                    ch = (char*)malloc(18);
                    sprintf(ch, "tel: %s\r\n", telephone_number);
                    uart_write_bytes(UART_OBD_PORT_NUM, ch, 18);
                    free(ch);
                    vTaskDelay(200 / portTICK_PERIOD_MS);

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
    char * message = (char*)malloc(1190);
    sprintf(message, "AT+HTTPPARA=URL,\"http://car-protector.herokuapp.com/postData?id=1&latitude=%.6lf&longitude=%.6lf&isActive=%d&dateOfLastStart=%02d-%02d-%d&timeOfLastStart=%d:%d&vehicleSpeed=%d&fuelLevel=%.1f&isMotion=%d\"\r", latitude, longitude, is_active, date_of_car_start.year, date_of_car_start.month, date_of_car_start.day, time_of_car_start.hour, time_of_car_start.minute, car_speed, fuel_level, motion_sensor);

    return message;
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

void Values::setMotionSensor(bool is_motion) {
    this->motion_sensor = is_motion;
}
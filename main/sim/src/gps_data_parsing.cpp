#include "gps_data_parsing.h"

GPSParsing::GPSParsing(int where): UartTalk(where) {
}

void GPSParsing::ActivateGps() {
    len = GPSParsing::UartConversation("AT+CGNSPWR=1\r", 500);
}

void GPSParsing::GetData() {
    len = GPSParsing::UartConversation("AT+CGNSINF\r", 10000);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
}

void GPSParsing::ParseData() {
    float year = 0;
    float month = 0;
    float day = 0;
    float hour = 0;
    float minute = 0;
    float second = 0;
    std::string latitude;
    std::string longitude;
    std::string altitude;
    std::string speed;
    std::string::size_type sz;

    char ch;
    uart_write_bytes(UART_OBD_PORT_NUM, "\r\n23 = ", 7);
    ch = buffer[23];
    uart_write_bytes(UART_OBD_PORT_NUM, &ch, 1);
    uart_write_bytes(UART_OBD_PORT_NUM, "\r\n", 2);
    vTaskDelay(50 / portTICK_PERIOD_MS);
    uart_write_bytes(UART_OBD_PORT_NUM, "\r\n24 = ", 7);
    ch = buffer[24];
    uart_write_bytes(UART_OBD_PORT_NUM, &ch, 1);
    uart_write_bytes(UART_OBD_PORT_NUM, "\r\n", 2);
    vTaskDelay(50 / portTICK_PERIOD_MS);

    if (isdigit(buffer[23])) {
        gps_data["GNSS run status"] = float(buffer[23] - '0');

        int num_of_commas = 0;
        int add_to_i = 0;
        int i = 24;
        int x = 0;
        //char ch;

        while (i < len-1 && num_of_commas < 7) {
            add_to_i = 0;

            if (buffer[i] == ',') {
                uart_write_bytes(UART_OBD_PORT_NUM, "gps comma\r\n", 11);
                ch = i;
                vTaskDelay(50 / portTICK_PERIOD_MS);
                uart_write_bytes(UART_OBD_PORT_NUM, &ch, 1);
                num_of_commas++;
                add_to_i = 1;
            }
            else {
                switch (num_of_commas) {
                    case 1:
                        uart_write_bytes(UART_OBD_PORT_NUM, "gps case 1\r\n", 12);
                        ch = i;
                        vTaskDelay(50 / portTICK_PERIOD_MS);
                        uart_write_bytes(UART_OBD_PORT_NUM, &ch, 1);
                        gps_data["Fix status"] = buffer[i] - '0';
                        add_to_i = 1;
                        break;
                    case 2:
                        uart_write_bytes(UART_OBD_PORT_NUM, "gps case 2\r\n", 12);
                        vTaskDelay(50 / portTICK_PERIOD_MS);
                        ch = i;
                        uart_write_bytes(UART_OBD_PORT_NUM, &ch, 1);
                        year = buffer[i] - '0';
                        for (int x = i+1; x < i+4; x++) {
                            year = year * 10 + (buffer[x] - '0');
                        }
                        gps_data["year"] = year;

                        month = buffer[i+4] - '0';
                        month = month * 10 + (buffer[i+5] - '0');
                        gps_data["month"] = month;

                        day = buffer[i+6] - '0';
                        day = day * 10 + (buffer[i+7] - '0');
                        gps_data["day"] = day;
                        
                        hour = buffer[i+8] - '0';
                        hour = hour * 10 + (buffer[i+9] - '0');
                        gps_data["hour"] = hour;

                        minute = buffer[i+10] - '0';
                        minute = minute * 10 + (buffer[i+11] - '0');
                        gps_data["minute"] = minute;
                        
                        second = buffer[i+12] - '0';
                        second = second * 10 + (buffer[i+13] - '0');
                        gps_data["second"] = second;

                        add_to_i = 18;
                        break;
                    case 3:
                        uart_write_bytes(UART_OBD_PORT_NUM, "gps case 3\r\n", 12);
                        ch = i;
                        vTaskDelay(50 / portTICK_PERIOD_MS);
                        uart_write_bytes(UART_OBD_PORT_NUM, &ch, 1);
                        x = i;
                        latitude = buffer[x];
                        x++;
                        while(buffer[x] != ',') {
                            latitude += buffer[x];
                            x++;
                        }
                        gps_data["latitude"] = std::stof(latitude, &sz);
                        add_to_i = x - i;

                        break;
                    case 4:
                        uart_write_bytes(UART_OBD_PORT_NUM, "gps case 4\r\n", 12);
                        ch = i;
                        vTaskDelay(50 / portTICK_PERIOD_MS);
                        uart_write_bytes(UART_OBD_PORT_NUM, &ch, 1);
                        x = i;
                        longitude = buffer[x];
                        x++;
                        while(buffer[x] != ',') {
                            longitude += buffer[x];
                            uart_write_bytes(UART_OBD_PORT_NUM, &buffer[x], 1);
                            vTaskDelay(100 / portTICK_PERIOD_MS);
                            x++;
                        }
                        gps_data["longitude"] = std::stof(longitude, &sz);
                        add_to_i = x - i;

                        break;
                    case 5:
                        uart_write_bytes(UART_OBD_PORT_NUM, "gps case 5\r\n", 12);
                        ch = i;
                        vTaskDelay(50 / portTICK_PERIOD_MS);
                        uart_write_bytes(UART_OBD_PORT_NUM, &ch, 1);
                        x = i;
                        altitude = buffer[x];
                        x++;
                        while(buffer[x] != ',') {
                            altitude += buffer[x];
                            x++;
                        }
                        gps_data["MLS altitude"] = std::stof(altitude, &sz);
                        add_to_i = x - i;

                        break;
                    case 6:
                        uart_write_bytes(UART_OBD_PORT_NUM, "gps case 6\r\n", 12);
                        ch = i;
                        vTaskDelay(50 / portTICK_PERIOD_MS);
                        uart_write_bytes(UART_OBD_PORT_NUM, &ch, 1);
                        x = i;
                        speed = buffer[x];
                        x++;
                        while(buffer[x] != ',') {
                            speed += buffer[x];
                            x++;
                        }
                        gps_data["speed"] = std::stof(speed, &sz);
                        add_to_i = x - i;

                        break;
                    default:
                        uart_write_bytes(UART_OBD_PORT_NUM, "def", 3);
                        vTaskDelay(1000 / portTICK_PERIOD_MS);
                        break;
                }
            }
            i = i + add_to_i;
        }
    }    
}

int GPSParsing::GetGNSSStatus() {
    return gps_data["GNSS run status"];
}

int GPSParsing::GetFixStatus() {
    return gps_data["Fix status"];
}

int GPSParsing::GetYear() {
    return gps_data["year"];
}

int GPSParsing::GetMonth() {
    return gps_data["month"];
}

int GPSParsing::GetDay() {
    return gps_data["day"];
}

int GPSParsing::GetHour() {
    return gps_data["hour"];
}

int GPSParsing::GetMinute() {
    return gps_data["minute"];
}

int GPSParsing::GetSecond() {
    return gps_data["second"];
}

float GPSParsing::GetLatitude() {
    return gps_data["latitude"];
}

float GPSParsing::GetLongitude() {
    return gps_data["longitude"];
}

float GPSParsing::GetAltitude() {
    return gps_data["MLS altitude"];
}

float GPSParsing::GetSpeed() {
    return gps_data["speed"];
}

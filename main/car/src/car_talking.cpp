#include "car_talking.h"
#include "sim_sms.h"

unsigned char CarTalking::checked_pids[] = {0b0000'0001, 0b0000'0001, 0b0000'0001, 0b0000'0001}; // engine_speed_active, vehicle_speed_active, runtime_active, fuel_tank_level_active
unsigned char CarTalking::active_pids[] = {0b0000'0000, 0b0000'0000, 0b0000'0000, 0b0000'0000};

void CarTalking::configureBluetooth() {
   bt.configure();
}

bool CarTalking::getObdStarted() {
    const char * device_name = "ATZ\r\r\rOBDII  v1.5";
    char buffer_to_read[17];

    readAndProcessMessage("ATZ\r", 4, buffer_to_read, 17, 5000);
    for (int i = 0; i < 17; i++) {
        if (device_name[i] != buffer_to_read[i]) {
            return false;
        }
    }

    return true;
}

bool CarTalking::setProtocol() {
    const char * response = "ATSP0\rOK";
    char buffer_to_read[8];

    readAndProcessMessage("ATSP0\r", 6, buffer_to_read, 8, 3000);
    for (int i = 0; i < 6; i++) {
        if (response[i] != buffer_to_read[i]) {
            return false;
        }
    }

    return true;
}

bool CarTalking::askPids1() {
    char buffer_to_read[7];

    readAndProcessMessage("0100\r",5, buffer_to_read, 7, 15000, true);
    uint8_t command_set[3] = {12, 13, 31};
    for (int i = 0; i < 3; i++) {
        if ((buffer_to_read[(command_set[i] / 8) + 4] >> (command_set[i] % 8)) & 1) {
            active_pids[i] |= checked_pids[i];
        }
    }
    return true;
}

bool CarTalking::askPids2() {
    char buffer_to_read[7];

    readAndProcessMessage("0120\r", 5, buffer_to_read, 7, 15000, true);
    uint8_t command_set = 15;
    if ((buffer_to_read[(command_set / 8) + 4] >> (command_set % 8)) & 1) {
        active_pids[3] |= checked_pids[3];
    }
    return true;
}

unsigned char* CarTalking::getActivePids() {
    return active_pids;
}

float CarTalking::askEngineSpeed() {
    char buffer_to_read[9];
    std::stringstream ss;
    int first_number;
    int second_number;

    readAndProcessMessage("010C\r", 5, buffer_to_read, 9, 15000, true);

    if (status == 0) {
        ss << std::hex << buffer_to_read[4] << buffer_to_read[5];
        ss >> first_number;

        std::stringstream().swap(ss);

        ss << std::hex << buffer_to_read[7] << buffer_to_read[8];
        ss >> second_number;

        return (float(first_number) * 256 + float(second_number)) / 4;
    }
    else if (status == 1) {
        return -1.0;
    }
    else {
        return 0.0;
    }    
}

int CarTalking::askVehicleSpeed() {
    char buffer_to_read[6];
    std::stringstream ss;
    int number;

    readAndProcessMessage("010D\r", 5, buffer_to_read, 6, 15000, true);

    if (status == 0) {
        ss << std::hex << buffer_to_read[4] << buffer_to_read[5];
        ss >> number;

        return number;
    }
    else if (status == 1) {
        return -1;
    }
    else {
        return 0;
    }
}

int CarTalking::askRuntime() {
    char buffer_to_read[9];
    std::stringstream ss;
    int first_number;
    int second_number;

    readAndProcessMessage("011F\r", 5, buffer_to_read, 9, 10000, true);

    if (status == 0) {
        ss << std::hex << buffer_to_read[4] << buffer_to_read[5];
        ss >> first_number;

        std::stringstream().swap(ss);
        
        ss << std::hex << buffer_to_read[7] << buffer_to_read[8];
        ss >> second_number;

        return (int)first_number * 256 + (int)second_number;
    }
    else if (status == 1) {
        return -1;
    }
    else {
        return 0;
    }
}

float CarTalking::askFuelLevel() {
    char buffer_to_read[6];
    std::stringstream ss;
    int number;

    readAndProcessMessage("012F\r", 5, buffer_to_read, 6, 10000, true);

    if (status == 0) {        
        ss << std::hex << buffer_to_read[4] << buffer_to_read[5];
        ss >> number;

        return ((float)number * 100) / 255;
    }
    else if (status == 1) {
        return -1.0;
    }
    else {
        return 0.0;
    }
}


void CarTalking::checkPidsSupported(uint8_t* command_set, int size) {
    char buffer_to_read[15];
    std::stringstream ss;
    char actual_bits[4];

    std::vector<std::string> pids{"0100\r", "0120\r"};

    for(int i = 0; i < 2; i++) {
        readAndProcessMessage(pids[i].c_str(), 5, buffer_to_read, 12, 10000, true);

        ss << std::hex << buffer_to_read[4] << buffer_to_read[5];
        ss >> actual_bits[0];

        std::stringstream().swap(ss);

        ss << std::hex << buffer_to_read[7] << buffer_to_read[8];
        ss >> actual_bits[1];

        std::stringstream().swap(ss);

        ss << std::hex << buffer_to_read[10] << buffer_to_read[11];
        ss >> actual_bits[2];

        std::stringstream().swap(ss);

        ss << std::hex << buffer_to_read[13] << buffer_to_read[14];
        ss >> actual_bits[3];

        for (int j = 0; j < size; j++) {
            if ((actual_bits[(command_set[j] / 8) + 4] >> (command_set[j] % 8)) & 1) {
                if (size == 1) {
                    active_pids[3] |= checked_pids[3];
                    return;
                }
                active_pids[i] |= checked_pids[i];
            }
        }
    }
}

void CarTalking::checkAvailableParams() {
    checkPidsSupported(new uint8_t[3]{12, 13, 31}, 3);
    checkPidsSupported(new uint8_t{15}, 1);
}

void CarTalking::readAndProcessMessage(const char* command_to_send, int len_of_command, char * buffer_to_read, int size_of_read_buffer, int wait_for, bool skip_info) {  
    std::pair<int, uint8_t*> pair;
    pair = bt.sendCommand(command_to_send, len_of_command, wait_for);
    
    int i = 0;
    std::string s;
    int num_of_chars = 0;
    int buffer_index = 0;
    int cnt = 0;
    int add_to_cnt = 0;
    while(i < pair.first) {
        num_of_chars = 0;
        
        if (pair.second[i] == ':' && i + 6 < pair.first) { // "\r\n"
            if (isdigit(pair.second[i+4])) {
                s = pair.second[i+4];
                num_of_chars = std::stoi(s);

                for (cnt = i+6; cnt < i + 6 + num_of_chars && cnt < pair.first; cnt++) {
                    cnt += add_to_cnt;
                    add_to_cnt = 0;

                    if (pair.second[cnt] == 'N' && pair.second[cnt+1] == 'O') {
                        status = 1;
                        return;
                    }
                    else if (pair.second[cnt] == 'U' && pair.second[cnt+1] == 'N') {
                        status = 2;
                        return;
                    }
                    if (skip_info) {
                        if (pair.second[cnt] == '4') {
                            skip_info = false;
                            add_to_cnt = 2;
                            if (cnt + 2 < i + 6 + num_of_chars) {
                                cnt += add_to_cnt;
                                add_to_cnt = 0;
                            }
                        }
                    }
                    if (buffer_index < size_of_read_buffer && !skip_info && add_to_cnt == 0) {
                        buffer_to_read[buffer_index] = pair.second[cnt];
                        buffer_index++;
                        uart_write_bytes(UART_OBD_PORT_NUM, &pair.second[cnt], 1);
                        vTaskDelay(100 / portTICK_PERIOD_MS);
                    }
                    
                }
                i = i + 6 + num_of_chars;
            }
        }
        else {
            i++;
        }
    }
}
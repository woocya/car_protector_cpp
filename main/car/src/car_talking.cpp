#include "car_talking.h"
#include "sim_sms.h"

unsigned char CarTalking::checked_pids[] = {0b0000'0001, 0b0000'0001, 0b0000'0001, 0b0000'0001}; // engine_speed_active, vehicle_speed_active, runtime_active, fuel_tank_level_active
unsigned char CarTalking::active_pids[] = {0b0000'0000, 0b0000'0000, 0b0000'0000, 0b0000'0000};

void CarTalking::configureBluetooth() {
   bt.Configure();
}

bool CarTalking::GetObdStarted() {
    const char * device_name = "ATZ\r\r\rOBDII  v1.5";
    char buffer_to_read[17];

    ReadAndProcessMessage("ATZ\r", 4, buffer_to_read, 17, 3000);
    for (int i = 0; i < 17; i++) {
        if (device_name[i] != buffer_to_read[i]) {
            return false;
        }
    }

    return true;
}

bool CarTalking::SetProtocol() {
    const char * response = "ATSP0\rOK";
    char buffer_to_read[8];

    ReadAndProcessMessage("ATSP0\r", 6, buffer_to_read, 8, 3000);
    for (int i = 0; i < 6; i++) {
        if (response[i] != buffer_to_read[i]) {
            return false;
        }
    }

    return true;
}

bool CarTalking::AskPids1() {
    char buffer_to_read[7];

    ReadAndProcessMessage("0100\r",5, buffer_to_read, 7, 10000, true);
    uint8_t command_set[3] = {12, 13, 31};
    for (int i = 0; i < 3; i++) {
        if ((buffer_to_read[(command_set[i] / 8) + 4] >> (command_set[i] % 8)) & 1) {
            active_pids[i] |= checked_pids[i];
        }
    }
    return true;
}

bool CarTalking::AskPids2() {
    char buffer_to_read[7];

    ReadAndProcessMessage("0120\r", 5, buffer_to_read, 7, 10000, true);
    uint8_t command_set = 15;
    if ((buffer_to_read[(command_set / 8) + 4] >> (command_set % 8)) & 1) {
        active_pids[3] |= checked_pids[3];
    }
    return true;
}

unsigned char* CarTalking::getActivePids() {
    return active_pids;
}

float CarTalking::AskEngineSpeed() {
    char buffer_to_read[20];
    std::string helper_string;
    std::stringstream ss;
    int first_number;
    int second_number;

    ReadAndProcessMessage("010C\r", 5, buffer_to_read, 20, 10000, true);

    helper_string = buffer_to_read[4];
    helper_string += buffer_to_read[5];

    ss << std::hex << helper_string;
    ss >> first_number;

    std::stringstream().swap(ss);

    helper_string = buffer_to_read[7];
    helper_string += buffer_to_read[8];

    ss << std::hex << helper_string;
    ss >> second_number;

    return (float(first_number) * 256 + float(second_number)) / 4;
}

int CarTalking::AskVehicleSpeed() {
    char buffer_to_read[6];
    std::string helper_string;
    std::stringstream ss;
    int number;

    ReadAndProcessMessage("010D\r", 5, buffer_to_read, 5, 8000);

    helper_string = buffer_to_read[4];
    helper_string += buffer_to_read[5];

    ss << std::hex << helper_string;
    ss >> number;

    return number;
}

int CarTalking::AskRuntime() {
    char buffer_to_read[9];    
    std::string helper_string;
    std::stringstream ss;
    int first_number;
    int second_number;

    ReadAndProcessMessage("011F\r", 5, buffer_to_read, 6, 8000);

    helper_string = buffer_to_read[4];
    helper_string += buffer_to_read[5];

    ss << std::hex << helper_string;
    ss >> first_number;

    std::stringstream().swap(ss);

    helper_string = buffer_to_read[7];
    helper_string += buffer_to_read[8];

    ss << std::hex << helper_string;
    ss >> second_number;

    return (int)first_number * 256 + (int)second_number;
}

float CarTalking::AskFuelLevel() {
    char buffer_to_read[6];
    std::string helper_string;
    std::stringstream ss;
    int number;

    ReadAndProcessMessage("012F\r", 5, buffer_to_read, 5, 8000);

    helper_string = buffer_to_read[4];
    helper_string += buffer_to_read[5];

    ss << std::hex << helper_string;
    ss >> number;

    return ((float)number * 100) / 255;
}


void CarTalking::CheckPidsSupported(uint8_t* command_set, int size) {
    char buffer_to_read[7];
    std::vector<std::string> pids{"0100\r", "0120\r"};

    for(int i = 0; i < 2; i++) {
        ReadAndProcessMessage(pids[i].c_str(), 5, buffer_to_read, 7, 10000, true);

            for (int j = 0; j < size; j++) {
                if ((buffer_to_read[(command_set[i] / 8) + 4] >> (command_set[i] % 8)) & 1) {
                    if (size == 1) {
                        active_pids[3] |= checked_pids[3];
                        return;
                    }
                    active_pids[i] |= checked_pids[i];
                }
            }
    }
}

void CarTalking::CheckAvailableParams() {
    CheckPidsSupported(new uint8_t[3]{12, 13, 31}, 3);
    CheckPidsSupported(new uint8_t{15}, 1);
}

void CarTalking::ReadAndProcessMessage(const char* command_to_send, int len_of_command, char * buffer_to_read, int size_of_read_buffer, int wait_for, bool skip_info) {  
    std::pair<int, uint8_t*> pair;
    pair = bt.SendCommand(command_to_send, len_of_command, wait_for);
    
    int i = 0;
    int num_of_chars = 0;
    int buffer_index = 0;
    int cnt = 0;
    while(i < pair.first) {
        num_of_chars = 0;
        
        if (pair.second[i] == ':' && i + 6 < pair.first) { // "\r\n"
            if (isdigit(pair.second[i+4])) {
                num_of_chars = pair.second[i+4] - '0';

                for (cnt = i+6; cnt < i + 6 + num_of_chars && cnt < pair.first; cnt++) {
                    if (skip_info) {
                        if (pair.second[cnt] == '4' && pair.second[cnt+1] == '1') {
                            skip_info = false;
                            cnt+=2;
                        }
                        else {
                            break;
                        }
                    }
                    buffer_to_read[buffer_index] = pair.second[cnt];
                    buffer_index++;
                    uart_write_bytes(UART_OBD_PORT_NUM, &pair.second[cnt], 1);
                }
                i = i + 6 + num_of_chars;
            }
        }
        else {
            i++;
        }
    }
}


// ustawić duży bufor, brać wszystko a potem czytać do /r i >, potem ew. zerować resztę - zrobione
// uart read - chyba lepiej pętla dla sprawdzania końca linii - zrobione
// error parsing
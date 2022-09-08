#include "car_talking.h"
#include "sim_sms.h"

unsigned char CarTalking::checked_pids[] = {0b0000'0001, 0b0000'0010, 0b0000'0100, 0b0000'1000}; // engine_speed_active, vehicle_speed_active, runtime_active, fuel_tank_level_active
unsigned char CarTalking::active_pids[] = {0b0000'0000, 0b0000'0000, 0b0000'0000, 0b0000'0000};


bool CarTalking::GetObdStarted() {
    const char * device_name = "\r\rOBDII  v1.5";
    char buffer_to_read[13];

    ReadAndProcessMessage("ATZ\r", 4, buffer_to_read, 13, 1000);
    // uart_write_bytes(UART_OBD_PORT_NUM, "reply:", 6);
    // uart_write_bytes(UART_OBD_PORT_NUM, buffer_to_read, 13);
    // uart_write_bytes(UART_OBD_PORT_NUM, "\n\r", 2);
    for (int i = 0; i < 12; i++) {
        if (device_name[i] != buffer_to_read[i]) {
            return false;
        }
    }

    return true;
}

bool CarTalking::TurnEchoOff() {
    int a = bt.Configure();

    const char * response = "ATE0\rOK";
    char buffer_to_read[7];

    ReadAndProcessMessage("ATE0\r", 5, buffer_to_read, 7, 1000);
    // uart_write_bytes(UART_OBD_PORT_NUM, "reply:", 6);
    // uart_write_bytes(UART_OBD_PORT_NUM, buffer_to_read, 7);
    // uart_write_bytes(UART_OBD_PORT_NUM, "\n\r", 2);
    for (int i = 0; i < 7; i++) {
        if (response[i] != buffer_to_read[i]) {
            return false;
        }
    }

    return true;
}

bool CarTalking::SetProtocol() {
    const char * response = "ATSP0\rOK";
    char buffer_to_read[8];

    ReadAndProcessMessage("ATSP0\r", 6, buffer_to_read, 8, 1000);
    for (int i = 0; i < 6; i++) {
        if (response[i] != buffer_to_read[i]) {
            return false;
        }
    }

    return true;
}

bool CarTalking::AskPids1() {
    char buffer_to_read[7];

    ReadAndProcessMessage("0100\r",5, buffer_to_read, 7, 8000);
    uint8_t command_set[3] = {12, 13, 31};
    for (int i = 0; i < 3; i++) {
        if ((buffer_to_read[command_set[i] / 8] >> (command_set[i] % 8)) & 1) {
            active_pids[i] |= checked_pids[i];
        }
    }
    return true;
}

bool CarTalking::AskPids2() {
    char buffer_to_read[7];

    ReadAndProcessMessage("0120\r", 5, buffer_to_read, 7, 8000);
    uint8_t command_set = 15;
    if ((buffer_to_read[command_set / 8] >> (command_set % 8)) & 1) {
        active_pids[3] |= checked_pids[3];
    }
    return true;
}

float CarTalking::AskEngineSpeed() {
    char buffer_to_read[20];
    ReadAndProcessMessage("010C\r", 5, buffer_to_read, 20, true, 15000);

    return ((float) buffer_to_read[9] * 256 + (float) buffer_to_read[1]) / 4;
}

int CarTalking::AskVehicleSpeed() {
    char buffer_to_read[5];
    ReadAndProcessMessage("010D\r", 5, buffer_to_read, 5, 8000);

    return (int) buffer_to_read[0];
}

float CarTalking::AskRuntime() {
    char buffer_to_read[6];
    ReadAndProcessMessage("011F\r", 5, buffer_to_read, 6, 8000);

    return (float) buffer_to_read[0] * 256 + (float) buffer_to_read[1];
}

float CarTalking::AskFuelLevel() {
    char buffer_to_read[5];
    ReadAndProcessMessage("012F\r", 5, buffer_to_read, 5, 8000);

    return ((float) buffer_to_read[0] * 100) / 255;
}


void CarTalking::CheckPidsSupported(uint8_t* command_set, int size) {
    uint8_t* supported_commands = (uint8_t*) buffer;
    for (int i = 0; i < size; i++) {
        if ((supported_commands[command_set[i] / 8] >> (command_set[i] % 8)) & 1) {
            if (size == 1) {
                active_pids[3] |= checked_pids[3];
                return;
            }
            active_pids[i] |= checked_pids[i];
        }
    }
}


void CarTalking::ReadAndProcessMessage(const char* command_to_send, int len_of_command, char * buffer_to_read, int size_of_read_buffer, int wait_for, bool skip_info) {  
    //int is_end_of_message = 0;
    //int chars_left_in_buffer = 0;
    std::pair<int, uint8_t*> pair;
    pair = bt.SendCommand(command_to_send, len_of_command, wait_for);
    
    int i = 0;
    int num_of_chars = 0;
    int buffer_index = 0;
    int cnt = 0;
    while(i < pair.first) {
        num_of_chars = 0;
        
        // uart_write_bytes(UART_OBD_PORT_NUM, &pair.second[i], 1);
        // std::string s = std::to_string(pair.first);
        // uart_write_bytes(UART_OBD_PORT_NUM, s.c_str(), 2);
        // uart_write_bytes(UART_OBD_PORT_NUM, " ", 1);
        if (pair.second[i] == ':' && i + 6 < pair.first) { // "\r\n"
            //is_end_of_message = 1;
            // if (buffer[i+2] == 0x3E && buffer[i+3] == 0x0D) { // ">\r" (is there \r at the end for sure???)
            //     is_prompt_char = 1;
            // }
            //chars_left_in_buffer = length - i+1; //for debugging

            if (isdigit(pair.second[i+4])) {
                num_of_chars = pair.second[i+4] - '0';
            // //char ch = num_of_chars - '0';          
            // uart_write_bytes(UART_OBD_PORT_NUM, &ch, 1);
            // uart_write_bytes(UART_OBD_PORT_NUM, "\r\n", 2);
            // ch = i+6 - '0';
            // uart_write_bytes(UART_OBD_PORT_NUM, &ch, 1);
           
        // s = std::to_string(i);
        // uart_write_bytes(UART_OBD_PORT_NUM, s.c_str(), 2);
        // uart_write_bytes(UART_OBD_PORT_NUM, "\r\n", 2);

                for (cnt = i+6; cnt < i + 6 + num_of_chars && cnt < pair.first; cnt++) {
                    if (skip_info) {
                        if (pair.second[cnt] == '4' && pair.second[cnt+1] == '1') {
                            skip_info = false;
                            continue;
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
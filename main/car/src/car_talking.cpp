#include "car_talking.h"
#include "sim_sms.h"

unsigned char CarTalking::checked_pids[] = {0b0000'0001, 0b0000'0010, 0b0000'0100, 0b0000'1000}; // engine_speed_active, vehicle_speed_active, runtime_active, fuel_tank_level_active
unsigned char CarTalking::active_pids[] = {0b0000'0000, 0b0000'0000, 0b0000'0000, 0b0000'0000};


bool CarTalking::GetObdStarted() {
    const char * device_name = "ELM327 v1.5\r\n>\r";
    char buffer_to_read[15];

    ReadAndProcessMessage("ATZ\r", 4, buffer_to_read, 15);
    for (int i = 0; i < 15; i++) {
        if (device_name[i] != buffer_to_read[i]) {
            return false;
        }
    }

    return true;
}

int CarTalking::TurnEchoOff() {
    int a = bt.Configure();

    const char * response = "OK\r\n>\r";
    char buffer_to_read[6];

    ReadAndProcessMessage("ATE0\r", 5, buffer_to_read, 6);
    for (int i = 0; i < 6; i++) {
        if (response[i] != buffer_to_read[i]) {
            return i+10;
        }
    }

    return a;
}

bool CarTalking::SetProtocol() {
    const char * response = "OK\r\n>\r";
    char buffer_to_read[6];

    ReadAndProcessMessage("ATSP3\r", 6, buffer_to_read, 6);
    for (int i = 0; i < 6; i++) {
        if (response[i] != buffer_to_read[i]) {
            return false;
        }
    }

    return true;
}

bool CarTalking::AskPids1() {
    char buffer_to_read[7];

    ReadAndProcessMessage("0x01000D", 8, buffer_to_read, 7);
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

    ReadAndProcessMessage("0x01200D", 8, buffer_to_read, 7);
    uint8_t command_set = 15;
    if ((buffer_to_read[command_set / 8] >> (command_set % 8)) & 1) {
        active_pids[3] |= checked_pids[3];
    }
    return true;
}

float CarTalking::AskEngineSpeed() {
    char buffer_to_read[6];
    ReadAndProcessMessage("0x010C0D", 8, buffer_to_read, 6);

    return ((float) buffer_to_read[0] * 256 + (float) buffer_to_read[1]) / 4;
}

int CarTalking::AskVehicleSpeed() {
    char buffer_to_read[5];
    ReadAndProcessMessage("0x010D0D", 8, buffer_to_read, 5);

    return (int) buffer_to_read[0];
}

float CarTalking::AskRuntime() {
    char buffer_to_read[6];
    ReadAndProcessMessage("0x011F0D", 8, buffer_to_read, 6);

    return (float) buffer_to_read[0] * 256 + (float) buffer_to_read[1];
}

float CarTalking::AskFuelLevel() {
    char buffer_to_read[5];
    ReadAndProcessMessage("0x012F0D", 8, buffer_to_read, 5);

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


void CarTalking::ReadAndProcessMessage(const char* command_to_send, int len_of_command, char * buffer_to_read, int size_of_read_buffer) {  
    //int is_end_of_message = 0;
    //int chars_left_in_buffer = 0;
    int length = bt.SendCommand(command_to_send, len_of_command);

    int i = 0;
    int num_of_chars = 0;
    int buffer_index = 0;
    while(i < length) {
        num_of_chars = 0;

        if (buffer[i] == ':' && i + 6 < length) { // "\r\n"
            //is_end_of_message = 1;
            // if (buffer[i+2] == 0x3E && buffer[i+3] == 0x0D) { // ">\r" (is there \r at the end for sure???)
            //     is_prompt_char = 1;
            // }
            //chars_left_in_buffer = length - i+1; //for debugging

            num_of_chars = buffer[i+4];
            for (int cnt = i+6; cnt < num_of_chars && cnt < length; cnt++) {
                if (buffer_index < size_of_read_buffer) {
                    buffer_to_read[buffer_index] = buffer[cnt];
                    buffer_index++;
                }
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
#include "car_talking.h"

CarTalking::CarTalking(int where): UartTalk(where) {
    //UartTalk talker(UART_OBD_PORT_NUM);
}

bool CarTalking::GetObdStarted() {
    const char * device_name = "ELM327 v1.5"; //is it for sure?
    int data_length = -1;
    while (data_length != -1) {
        ReadAndProcessMessage("ATZ0x0D", 12);
    }
    for (int i = 0; i < 12; i++) {
        if (device_name[i] != buffer[i]) {
            return false;
        }
    }

    return true;
}

bool CarTalking::TurnEchoOff() {
    const char * response = "OK";
    int data_length = -1;
    while (data_length != -1) {
        ReadAndProcessMessage("ATSP30x0D", 2);
    }
    for (int i = 0; i < 2; i++) {
        if (response[i] != buffer[i]) {
            return false;
        }
    }

    return true;
}

bool CarTalking::SetProtocol() {
    const char * response = "OK";
    int data_length = -1;
    while (data_length != -1) {
        ReadAndProcessMessage("ATSP30x0D", 2);
    }
    for (int i = 0; i < 2; i++) {
        if (response[i] != buffer[i]) {
            return false;
        }
    }

    return true;
}

bool CarTalking::AskPids1() {
    int data_length = -1;
    while (data_length != -1) {
        ReadAndProcessMessage("0x01000D", 4);
    }
    uint8_t command_set[3] = {12, 13, 31};
    for (int i = 0; i < 3; i++) {
        if ((buffer[command_set[i] / 8] >> (command_set[i] % 8)) & 1) {
            active_pids[i] |= checked_pids[i];
        }
    }
    return true;
}

bool CarTalking::AskPids2() {
    int data_length = -1;
    while (data_length != -1) {
        ReadAndProcessMessage("0x01200D", 4);
    }
    uint8_t command_set = 15;
    if ((buffer[command_set / 8] >> (command_set % 8)) & 1) {
        active_pids[3] |= checked_pids[3];
    }
    return true;
}

float CarTalking::AskEngineSpeed() {
    int data_length = -1;
    while (data_length != -1) {
        ReadAndProcessMessage("0x010C0D", 2);
    }
    return ((float) buffer[0] * 256 + (float) buffer[1]) / 4;
}

float CarTalking::AskVehicleSpeed() {
    int data_length = -1;
    while (data_length != -1) {
        ReadAndProcessMessage("0x010D0D", 1);
    }
    return (float) buffer[0];
}

float CarTalking::AskRuntime() {
    int data_length = -1;
    while (data_length != -1) {
        ReadAndProcessMessage("0x011F0D", 2);
    }
    return (float) buffer[0] * 256 + (float) buffer[1];
}

float CarTalking::AskFuelLevel() {
    int data_length = -1;
    while (data_length != -1) {
        ReadAndProcessMessage("0x012F0D", 1);
    }
    return ((float) buffer[0] * 100) / 255;
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


int CarTalking::ReadAndProcessMessage(const char* command_to_send, int expected_bytes) {  
    int is_end_of_message = 0;
    //int chars_left_in_buffer = 0;
    int length = 0;
    while (length != 0) { //that seems like a terrible idea
        length = UartConversation(command_to_send, 500);
    }
    
    for (int i=0; i<length; i++) {
        if (buffer[i] == 0x0D && buffer[i+1] == 0x0A) { // "\r\n"
            is_end_of_message = 1;
            if (buffer[i+2] == 0x3E && buffer[i+3] == 0x0D) { // ">\r" (is there \r at the end for sure???)
                is_prompt_char = 1;
            }
            //chars_left_in_buffer = length - i+1; //for debugging
        }
    }
    
    if (is_end_of_message == 0) {
        return -1; // no end of message - what then? then repeat reading from parent function
    }

    return length;
}

// ustawić duży bufor, brać wszystko a potem czytać do /r i >, potem ew. zerować resztę - zrobione
// uart read - chyba lepiej pętla dla sprawdzania końca linii - zrobione
// error parsing
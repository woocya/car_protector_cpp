#include "car_talking.h"

CarTalking::CarTalking(int where): UartTalk(where) {
    UartTalk talker(UART_OBD_PORT_NUM);
    // is_prompt_char = 0;
    // // Configure buffer for the incoming data
    // uint8_t d_from_c[IN_BUF_SIZE];
    // std::fill_n(d_from_c[0], IN_BUF_SIZE-1, 0);
    // data_from_car = &d_from_c[0];
    
    // // Configure buffer for the outcoming data
    // uint8_t d_for_c[OUT_BUF_SIZE];
    // std::fill_n(d_for_c[0], OUT_BUF_SIZE-1, 0);
    // data_for_car = &d_for_c[0];
}

// int CarTalking::CountBytes() { // idk if it's a good idea
//     int how_many = 0;
//     for (int i = 0; i < IN_BUF_SIZE; i++) {
//         how_many++;
//         if (data_from_car[i] == 0x0D) {
//             return how_many;
//         }
//     }
//     return -1;
// }

// bool CarTalking::TalkToObd(PId command_to_send) { // better bool or void?

//     UartWrite(command_to_send);
//     if (command_to_send.GetPidExpectedBytes() > 0) {
//         UartRead(command_to_send.GetPidExpectedBytes());
//     }    
//     return true;
// }

// int CarTalking::GetCarInfo(PId command_to_send) {
//     UartWrite(command_to_send);
//     UartRead(command_to_send.GetPidExpectedBytes());
//     return 0;
// } 

float CarTalking::TranslateInfo(PId sent_command) { //could be done better if usage of some other container than plain array would be reasonably possible
    if (sent_command.GetParameterName() == "pids supported 1") { //pids supported 1
        uint8_t command_set[3] = {12, 13, 31};
        uint8_t* command_set_ptr = &command_set[0];
        CarTalking::CheckPidsSupported(command_set_ptr, 3);
    }
    else if (sent_command.GetPidCommand() == "pids supported 2") { //pids supported 2
        uint8_t command_set = 15;
        uint8_t* command_set_ptr = &command_set;
        CarTalking::CheckPidsSupported(command_set_ptr, 1);
    }
    else if (sent_command.GetPidCommand() == "engine speed") { 
        return ((float) buffer[0] * 256 + (float) buffer[1]) / 4;
    }
    else if (sent_command.GetPidCommand() == "vehicle speed") {
        return (float) buffer[0];
    }
    else if (sent_command.GetPidCommand() == "run time since engine start") {
        return (float) buffer[0] * 256 + (float) buffer[1];
    }
    else if (sent_command.GetPidCommand() == "fuel tank level input") {
        return ((float) buffer[0] * 100) / 255;
    }
    return 0;
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


int CarTalking::ProcessMessage(PId command_to_send, int expected_bytes) {  
    // int is_end_of_message = 0;
    // int chars_left_in_buffer = 0;
    // is_prompt_char = 0;
    // uart_flush(UART_PORT_NUM);
    // int length = 0;
    // //check how many bytes waiting in the buffer
    // ESP_ERROR_CHECK(uart_get_buffered_data_len(UART_PORT_NUM, (size_t*)&length));
    // if (length >= IN_BUF_SIZE) { 
    //     return -1; // too little buffer
    // }
    // // Read data from the UART
    // uart_read_bytes(UART_PORT_NUM, data_from_car, length, 20 / portTICK_RATE_MS);
    int is_end_of_message = 0;
    int chars_left_in_buffer = 0;
    int length = 0;
    while (length != 0) { //that seems like a terrible idea
        length = UartConversation(command_to_send.GetPidCommand(), 500);
    }
    
    for (int i=0; i<length; i++) {
        if (buffer[i] == 0x0D && buffer[i+1] == 0x0A) { // "\r\n"
            is_end_of_message = 1;
            if (buffer[i+2] == 0x3E && buffer[i+3] == 0x0D) { // ">\r" (is there \r at the end for sure???)
                is_prompt_char = 1;
            }
            chars_left_in_buffer = length - i+1; //for debugging
        }
    }
    
    if (is_end_of_message == 0) {
        return -1; // no end of message - what then? then repeat reading from parent function
    }

    //std::fill_n(data_from_car[length-1], IN_BUF_SIZE-1, 0);
    return length;
}

// bool CarTalking::UartWrite(PId pid) {    
//     if (is_prompt_char == 0) {
//         int temp_length = 0;
//         uint8_t temp_buffor[2];
//         while (temp_buffor[0] != 0x3E && temp_buffor[1] != 0x0D) { // wait for prompt character
//             // wait for some time first
//             ESP_ERROR_CHECK(uart_get_buffered_data_len(UART_PORT_NUM, (size_t*)&temp_length));
//             if (temp_length > 0) {
//                 uart_read_bytes(UART_PORT_NUM, temp_buffor, temp_length, 20 / portTICK_RATE_MS);
//             }
//         }
//     }
    
//     data_for_car = (uint8_t *) pid.GetPidCommand();
//     uart_flush(UART_PORT_NUM); //flush buffer to be sure there's no accidental garbage
//     uart_write_bytes(UART_PORT_NUM, (const char *) data_for_car, OUT_BUF_SIZE);
//     return true;
// }

// ustawić duży bufor, brać wszystko a potem czytać do /r i >, potem ew. zerować resztę - zrobione
// uart read - chyba lepiej pętla dla sprawdzania końca linii - zrobione
// error parsing
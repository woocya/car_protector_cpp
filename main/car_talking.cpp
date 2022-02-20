#include "car_talking.h"

CarTalking::CarTalking() {
    is_prompt_char = 0;
    // Configure buffer for the incoming data
    uint8_t d_from_c[IN_BUF_SIZE];
    std::fill_n(d_from_c[0], IN_BUF_SIZE-1, 0);
    data_from_car = &d_from_c[0];
    
    // Configure buffer for the outcoming data
    uint8_t d_for_c[OUT_BUF_SIZE];
    std::fill_n(d_for_c[0], OUT_BUF_SIZE-1, 0);
    data_for_car = &d_for_c[0];
}

int CarTalking::CountBytes() { // idk if it's a good idea
    int how_many = 0;
    for (int i = 0; i < IN_BUF_SIZE; i++) {
        how_many++;
        if (data_from_car[i] == 0x0D) {
            return how_many;
        }
    }
    return -1;
}

bool CarTalking::TalkToObd(PId command_to_send) { // better bool or void?

    UartWrite(command_to_send);
    if (command_to_send.GetPidExpectedBytes() > 0) {
        UartRead(command_to_send.GetPidExpectedBytes());
    }    
    return true;
}

int CarTalking::GetCarInfo(PId command_to_send) {
    UartWrite(command_to_send);
    UartRead(command_to_send.GetPidExpectedBytes());
    return 0;
} 

float CarTalking::TranslateInfo(PId sent_command) { //could be done better if usage of some other container than plain array would be reasonably possible
    if (strcmp(sent_command.GetPidCommand(), "0x01000D")) {
        uint8_t command_set[3] = {12, 13, 31};
        uint8_t* command_set_ptr = &command_set[0];
        CarTalking::CheckPidsSupported(command_set_ptr, 3);
    }
    else if (strcmp(sent_command.GetPidCommand(), "0x01200D")) {
        uint8_t command_set = 15;
        uint8_t* command_set_ptr = &command_set;
        CarTalking::CheckPidsSupported(command_set_ptr, 1);
    }
    else if (strcmp(sent_command.GetPidCommand(), "0x010C0D")) {
        return ((float) data_from_car[0] * 256 + (float) data_from_car[1]) / 4;
    }
    else if (strcmp(sent_command.GetPidCommand(), "0x010D0D")) {
        return (float) data_from_car[0];
    }
    else if (strcmp(sent_command.GetPidCommand(), "0x011F0D")) {
        return (float) data_from_car[0] * 256 + (float) data_from_car[1];
    }
    else if (strcmp(sent_command.GetPidCommand(), "0x012F0D")) {
        return ((float) data_from_car[0] * 100) / 255;
    }
    return 0;
}

void CarTalking::CheckPidsSupported(uint8_t* command_set, int size) {
    uint8_t* supported_commands = (uint8_t*) data_from_car;
    for (int i = 0; i < size; i++) {
        if ((supported_commands[command_set[i] / 8] >> (command_set[i] % 8)) & 1) {
            if (size == 1) {
                active_pids |= checked_pids[3];
                return;
            }
            active_pids |= checked_pids[i];
        }
    }
}

bool CarTalking::UartConfig() { // taken from esp examples
    uart_config_t uart_config = {
        .baud_rate = UART_BAUD_RATE,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_APB,
    };
    int intr_alloc_flags = 0;

#if CONFIG_UART_ISR_IN_IRAM
    intr_alloc_flags = ESP_INTR_FLAG_IRAM;
#endif

    ESP_ERROR_CHECK(uart_driver_install(UART_PORT_NUM, IN_BUF_SIZE * 2, 0, 0, NULL, intr_alloc_flags));
    ESP_ERROR_CHECK(uart_param_config(UART_PORT_NUM, &uart_config));
    ESP_ERROR_CHECK(uart_set_pin(UART_PORT_NUM, UART_TXD_PIN_NUM, UART_RXD_PIN_NUM, UART_RTS_PIN_NUM, UART_CTS_PIN_NUM));
    return true;
}

int CarTalking::UartRead(int expected_bytes) {    
    is_prompt_char = 0;
    uart_flush(UART_PORT_NUM);
    int length = 0;
    //check how many bytes waiting in the buffer
    ESP_ERROR_CHECK(uart_get_buffered_data_len(UART_PORT_NUM, (size_t*)&length));
    if (length >= IN_BUF_SIZE) { 
        return -1; // too little buffer
    }
    // Read data from the UART
    uart_read_bytes(UART_PORT_NUM, data_from_car, length, 20 / portTICK_RATE_MS);
    if (data_from_car[length-4] == 0x0D && data_from_car[length-3] == 0x0A) { // "\r\n"
        if (data_from_car[length-2] == 0x3E && data_from_car[length-1] == 0x0D) { // ">\r" (is there \r at the end for sure???)
            is_prompt_char = 1;
        }
    }
    else if (data_from_car[length-2] != 0x0D && data_from_car[length-1] != 0x0A) {
        return -1; // no end of message - what then?
    }
    
    std::fill_n(data_from_car[length-1], IN_BUF_SIZE-1, 0);
    return length;
}

bool CarTalking::UartWrite(PId pid) {    
    uint8_t temp_buffor[4];
    int temp_length = 0;
    ESP_ERROR_CHECK(uart_get_buffered_data_len(UART_PORT_NUM, (size_t*)&temp_length));
    uart_read_bytes(UART_PORT_NUM, temp_buffor, temp_length, 20 / portTICK_RATE_MS);
    if (is_prompt_char == 0) {
        while (temp_buffor[temp_length-2] != 0x3E && temp_buffor[temp_length-1] != 0x0D) { // wait for prompt character
            // wait for some time first
            ESP_ERROR_CHECK(uart_get_buffered_data_len(UART_PORT_NUM, (size_t*)&temp_length));
            uart_read_bytes(UART_PORT_NUM, temp_buffor, temp_length, 20 / portTICK_RATE_MS);
        }
    }
    
    data_for_car = (uint8_t *) pid.GetPidCommand();
    uart_flush(UART_PORT_NUM); //flush buffer to be sure there's no accidental garbage
    uart_write_bytes(UART_PORT_NUM, (const char *) data_for_car, OUT_BUF_SIZE);
    return true;
}

// ustawić duży bufor, brać wszystko a potem czytać do /r i >, potem ew. zerować resztę
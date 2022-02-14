#include "car_talking.h"

CarTalking::CarTalking() {
    // Configure a temporary buffer for the incoming data
    data_from_car = (uint8_t *) malloc(BUF_SIZE);
    
    // Configure a temporary buffer for the outcoming data
    data_for_car = (uint8_t *) malloc(BUF_SIZE);
}

CarTalking::~CarTalking() {
    free(data_from_car);
    free(data_for_car);
}

bool CarTalking::CheckConnection() {
    PId get_dev_name(12, "ATZ0x0D", "get device name");
    UartWrite(get_dev_name);
    UartRead(12);
    return true;
} 

bool CarTalking::SetProtocol() { //only use after turning echo off
    PId protocol(0, "ATSP20x0D", "set protocol to SAE J1850 VPW (10.4 kbaud)");
    UartWrite(protocol);
    return true;
} //atsp2 lub atsp3   

bool CarTalking::TurnEchoOff() {
    PId protocol(0, "ATE00x0D", "turn echo off");
    UartWrite(protocol);
    return true;
}

int CarTalking::GetInfo(PId command) {
    UartWrite(command);
    UartRead(command.GetPidExpectedBytes());
    return 0;
} 

int CarTalking::TranslateInfo(PId sent_command) {

    return 0;
}

bool CarTalking::UartConfig() {
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

    ESP_ERROR_CHECK(uart_driver_install(UART_PORT_NUM, BUF_SIZE * 2, 0, 0, NULL, intr_alloc_flags));
    ESP_ERROR_CHECK(uart_param_config(UART_PORT_NUM, &uart_config));
    ESP_ERROR_CHECK(uart_set_pin(UART_PORT_NUM, UART_TXD_PIN_NUM, UART_RXD_PIN_NUM, UART_RTS_PIN_NUM, UART_CTS_PIN_NUM));
    return true;
}

int CarTalking::UartRead(int expected_bytes) {    
    uart_flush(UART_PORT_NUM);
    int length = 0;
    //check how many bytes waiting in the buffer
    //ESP_ERROR_CHECK(uart_get_buffered_data_len(UART_PORT_NUM, (size_t*)&length));
    // Read data from the UART
    uart_read_bytes(UART_PORT_NUM, data_from_car, expected_bytes, 20 / portTICK_RATE_MS);
    return length;
}

void CarTalking::UartWrite(PId pid) {    //add checking for prompt character (">" - hex 3E)
    uart_flush(UART_PORT_NUM); //flush buffer to be sure 
    data_for_car = (uint8_t *) pid.GetPidCommand();
    uart_write_bytes(UART_PORT_NUM, (const char *) data_for_car, BUF_SIZE);
}
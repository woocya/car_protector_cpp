#include "car_talking.h"

CarTalking::CarTalking() {
    // Configure a temporary buffer for the incoming data
    dataFromCar = (uint8_t *) malloc(BUF_SIZE);
    
    // Configure a temporary buffer for the outcoming data
    dataForCar = (uint8_t *) malloc(BUF_SIZE);
}

bool CarTalking::checkConnection() {
    PId getDevName("ATZ0x0D", "get device name");
    uartWrite(getDevName);
    uartRead();
    return true;
} 

bool CarTalking::setProtocol() {
    PId protocol("ATSP20x0D", "set protocol to SAE J1850 VPW (10.4 kbaud)");
    uartWrite(protocol);
    uartRead();
    return true;
} //atsp2 lub atsp3   

int CarTalking::getInfo(PId command, int expected_bits) {
    uartWrite(command);
    uartRead();
    return 0;
} 

int CarTalking::translateInfo() {return 0;}

bool CarTalking::uartConfig() {
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

void CarTalking::uartRead() {    
    uart_flush(UART_PORT_NUM);
    int length = 0;
    ESP_ERROR_CHECK(uart_get_buffered_data_len(UART_PORT_NUM, (size_t*)&length));
    // Read data from the UART
    uart_read_bytes(UART_PORT_NUM, dataFromCar, length, 20 / portTICK_RATE_MS);
}

void CarTalking::uartWrite(PId pid) {    
    uart_flush(UART_PORT_NUM);
    dataForCar = (uint8_t *) pid.get_pid_message();
    uart_write_bytes(UART_PORT_NUM, (const char *) dataForCar, BUF_SIZE);
}
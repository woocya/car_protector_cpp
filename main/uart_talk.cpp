#include "uart_talk.h"

UartTalk::UartTalk(int where) {
    buffer = new uint8_t(UART_BUF_SIZE);
    this->where = where;
}

UartTalk::~UartTalk() {
    free(buffer);
}

int UartTalk::UartConversation(const char *command, int wait_for) {
    int len = 0;

    uart_write_bytes(where, command, strlen(command));
    vTaskDelay(wait_for / portTICK_PERIOD_MS);
        
    if (buffer != NULL) {
        ESP_ERROR_CHECK(uart_get_buffered_data_len(where, (size_t*)&len));
        uart_read_bytes(where, (void *)buffer, len, 20 / portTICK_RATE_MS);
    }    
    return len;
}

//what to do if len == -1?
#include "uart_talk.h"

bool status = 0;

UartTalk::UartTalk(int where) {
    buffer = new uint8_t[UART_BUF_SIZE];
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

        char arr[len];
        for (int a = 0; a < len; a++) {
            arr[a] = buffer[a];
        }
        
        uart_write_bytes(UART_OBD_PORT_NUM, "buffer is:", 10);
        vTaskDelay(100 / portTICK_PERIOD_MS);
        uart_write_bytes(UART_OBD_PORT_NUM, &arr, len);
        vTaskDelay(200 / portTICK_PERIOD_MS);
        uart_write_bytes(UART_OBD_PORT_NUM, "end\n\r", 5);
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
    return len;
}
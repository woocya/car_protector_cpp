#include "sim_sms.h"

//Sim::Sim(int where): UartTalk(where) {}

bool Sim::InitializeSIM() {
    gpio_reset_pin(GPIO_NUM_19);
    /* Set the GPIO as a push/pull output */
    gpio_set_direction(GPIO_NUM_19, GPIO_MODE_OUTPUT);
    gpio_set_level(GPIO_NUM_19, 0);
    vTaskDelay(4000 / portTICK_PERIOD_MS);
    gpio_set_level(GPIO_NUM_19, 1);
    //gpio_reset_pin(15);
    vTaskDelay(15000 / portTICK_PERIOD_MS);

    UartConversation("AT\r", 500);

    return true;
}

void Sim::SetTelephoneNumber(char* telephone_number) {
    int i = 0;
    while (i < 9) {
        this->telephone_number[i] = telephone_number[i];
        i++;
    }
}

bool Sim::SendSMS(const char * what_to_send) {
    Sim::UartConversation("AT+CSCA=\"+48501200777\"\r", 500);
    Sim::UartConversation("AT+CMGF=1\r", 500);
    char * ch = (char*)malloc(21);
    sprintf(ch, "AT+CMGS=\"%c%c%c%c%c%c%c%c%c\"\r", telephone_number[0], telephone_number[1], telephone_number[2], telephone_number[3], telephone_number[4], telephone_number[5], telephone_number[6], telephone_number[7], telephone_number[8]);
    Sim::UartConversation(ch, 500);

    uint8_t ctrlz = 0x1A;
    UartConversation(what_to_send, 5000);
    uart_write_bytes(UART_SIM_PORT_NUM, &ctrlz, 1);
    vTaskDelay(10000 / portTICK_PERIOD_MS);
    free(ch);
    return true;
}
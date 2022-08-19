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

    // TalkAndCheck("AT+IPR=9600\r", 500, "OK");
    TalkAndCheck("AT\r", 500, "OK");
    //TalkAndCheck("ATE0\r", 500, "OK");

    return true;
}

bool Sim::InitializeGPRS() {
    Sim::TalkAndCheck("AT+CPIN?\r", 500, "+CPIN: READY\r\nOK");
    Sim::TalkAndCheck("AT+CSTT=\"internet\",\"internet\",\"internet\"\r", 500, "OK");
    Sim::TalkAndCheck("AT+CIICR\r", 500, "OK");
    Sim::TalkAndCheck("AT+CIPSTART=\"TCP\",\"exploreembedded.com\",80\r", 5000, "OK\r\nCONNECT OK");

    return true;
}

bool Sim::SendSMS(const char * what_to_send) {
    Sim::TalkAndCheck("AT+CSCA=\"+48501200777\"\r", 500, "OK");
    Sim::TalkAndCheck("AT+CMGF=1\r", 500, "OK");
    Sim::TalkAndCheck("AT+CMGS=\"539399959\"\r", 500, ">");

    // char* command = new char[strlen(what_to_send) + 1];
    // sprintf(command, "%s%X", what_to_send, 0x1A);
    uint8_t ctrlz = 0x1A;
    TalkAndCheck(what_to_send, 5000, NULL);
    uart_write_bytes(UART_SIM_PORT_NUM, &ctrlz, 1);
    vTaskDelay(10000 / portTICK_PERIOD_MS);
    //free(command);
    return true;
}

void Sim::SendPackage(int length, const char* what_to_send) {
    char* command = new char[14];
    sprintf(command, "AT+CIPSEND=%d\r", length);
    TalkAndCheck(command, 500, ">");
    free(command);

    command = new char[strlen(what_to_send) + 1];
    sprintf(command, "SET %s HTTP/1.0", what_to_send);
    TalkAndCheck(command, 5000, "SEND OK");
    free(command);
}

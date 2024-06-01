#include "database_talking.h"

DatabaseTalking::DatabaseTalking(int where): UartTalk(where) {}

void DatabaseTalking::activateGPRS() {
    DatabaseTalking::uartConversation("AT+SAPBR=3,1,Contype,GPRS\r", 500);
    DatabaseTalking::uartConversation("AT+SAPBR=3,1,APN,\"internet\"\r", 500);
    DatabaseTalking::uartConversation("AT+SAPBR=3,1,USER,\"internet\"\r", 500);
    DatabaseTalking::uartConversation("AT+SAPBR=3,1,PWD,\"internet\"\r", 500);
}

char * DatabaseTalking::getDataFromDatabase() {
    DatabaseTalking::uartConversation("AT+SAPBR=1,1\r", 700);
    DatabaseTalking::uartConversation("AT+HTTPINIT\r", 700);
    DatabaseTalking::uartConversation("AT+HTTPPARA=CID,1\r", 700);
    DatabaseTalking::uartConversation("AT+HTTPPARA=URL,\"http://car-protector.herokuapp.com/getLimits\"\r", 700);
    DatabaseTalking::uartConversation("AT+HTTPDATA=192,5000\r", 3000);
    vTaskDelay(3000 / portTICK_PERIOD_MS);
    DatabaseTalking::uartConversation("AT+HTTPACTION=0\r", 5000);
    vTaskDelay(3000 / portTICK_PERIOD_MS);
    int len = DatabaseTalking::uartConversation("AT+HTTPREAD\r", 3000);

    char* response = (char*)malloc(len);
    for (int i = 0; i < len; i++) {
        response[i] = buffer[i];
    }

    DatabaseTalking::uartConversation("AT+HTTPTERM\r", 700);
    DatabaseTalking::uartConversation("AT+SAPBR=0,1\r", 700);

    return response;
}

void DatabaseTalking::sendDataToDatabase(const char * url) {
    DatabaseTalking::uartConversation("AT+SAPBR=1,1\r", 3000);
    DatabaseTalking::uartConversation("AT+HTTPINIT\r", 700);
    DatabaseTalking::uartConversation("AT+HTTPPARA=CID,1\r", 700);
    DatabaseTalking::uartConversation(url, 5000);
    DatabaseTalking::uartConversation("AT+HTTPDATA=0,5000\r", 1000);
    DatabaseTalking::uartConversation("AT+HTTPACTION=1\r", 5000);
    DatabaseTalking::uartConversation("AT+HTTPREAD\r", 700);
    DatabaseTalking::uartConversation("AT+HTTPTERM\r", 700);
    DatabaseTalking::uartConversation("AT+SAPBR=0,1\r", 700);
}
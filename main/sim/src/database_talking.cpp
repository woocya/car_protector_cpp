#include "database_talking.h"

DatabaseTalking::DatabaseTalking(int where): UartTalk(where) {}

void DatabaseTalking::ActivateGPRS() {
    DatabaseTalking::UartConversation("AT+SAPBR=3,1,Contype,GPRS\r", 500);
    DatabaseTalking::UartConversation("AT+SAPBR=3,1,APN,\"internet\"\r", 500);
    DatabaseTalking::UartConversation("AT+SAPBR=3,1,USER,\"internet\"\r", 500);
    DatabaseTalking::UartConversation("AT+SAPBR=3,1,PWD,\"internet\"\r", 500);
}

char * DatabaseTalking::GetDataFromDatabase() {
    DatabaseTalking::UartConversation("AT+SAPBR=1,1\r", 700);
    DatabaseTalking::UartConversation("AT+HTTPINIT\r", 700);
    DatabaseTalking::UartConversation("AT+HTTPPARA=CID,1\r", 700);
    DatabaseTalking::UartConversation("AT+HTTPPARA=URL,\"http://car-protector.herokuapp.com/getLimits\"\r", 700);
    DatabaseTalking::UartConversation("AT+HTTPDATA=192,5000\r", 3000);
    vTaskDelay(3000 / portTICK_PERIOD_MS);
    DatabaseTalking::UartConversation("AT+HTTPACTION=0\r", 5000);
    vTaskDelay(3000 / portTICK_PERIOD_MS);
    int len = DatabaseTalking::UartConversation("AT+HTTPREAD\r", 3000);

    char* response = (char*)malloc(len);
    for (int i = 0; i < len; i++) {
        response[i] = buffer[i];
    }

    DatabaseTalking::UartConversation("AT+HTTPTERM\r", 700);
    DatabaseTalking::UartConversation("AT+SAPBR=0,1\r", 700);

    return response;
}

void DatabaseTalking::SendDataToDatabase(const char * url) {
    DatabaseTalking::UartConversation("AT+SAPBR=1,1\r", 3000);
    DatabaseTalking::UartConversation("AT+HTTPINIT\r", 700);
    DatabaseTalking::UartConversation("AT+HTTPPARA=CID,1\r", 700);
    DatabaseTalking::UartConversation(url, 5000);
    DatabaseTalking::UartConversation("AT+HTTPDATA=0,5000\r", 1000);
    DatabaseTalking::UartConversation("AT+HTTPACTION=1\r", 5000);
    DatabaseTalking::UartConversation("AT+HTTPREAD\r", 700);
    DatabaseTalking::UartConversation("AT+HTTPTERM\r", 700);
    DatabaseTalking::UartConversation("AT+SAPBR=0,1\r", 700);
}
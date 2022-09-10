#include "database_talking.h"

DatabaseTalking::DatabaseTalking(int where): UartTalk(where) {}

void DatabaseTalking::ActivateGPRS() {
    DatabaseTalking::TalkAndCheck("AT+SAPBR=3,1,Contype,GPRS\r", 500, "OK");
    DatabaseTalking::TalkAndCheck("AT+SAPBR=3,1,APN,\"internet\"\r", 500, "OK");
    DatabaseTalking::TalkAndCheck("AT+SAPBR=3,1,USER,\"internet\"\r", 500, "OK");
    DatabaseTalking::TalkAndCheck("AT+SAPBR=3,1,PWD,\"internet\"\r", 500, "OK");
}

void DatabaseTalking::SendRequestToUser() {
    DatabaseTalking::TalkAndCheck("AT+SAPBR=1,1\r", 500, "OK");
    DatabaseTalking::TalkAndCheck("AT+HTTPINIT\r", 500, "OK");
    DatabaseTalking::TalkAndCheck("AT+HTTPPARA=CID,1\r", 500, "OK");
    DatabaseTalking::TalkAndCheck("AT+HTTPPARA=URL,\"http://car-protector.herokuapp.com/postCoordinates\"\r", 500, "OK");
    DatabaseTalking::TalkAndCheck("AT+HTTPDATA=192,5000\r", 1000, "OK");
    DatabaseTalking::TalkAndCheck("id=1, latitude= , longitude=", 5000, "OK");
    DatabaseTalking::TalkAndCheck("AT+HTTPACTION=1\r", 5000, "OK");
    DatabaseTalking::TalkAndCheck("AT+HTTPREAD\r", 500, "OK");
    DatabaseTalking::TalkAndCheck("AT+HTTPTERM\r", 500, "OK");
    DatabaseTalking::TalkAndCheck("AT+SAPBR=0,1\r", 500, "OK");
}

char * DatabaseTalking::GetDataFromDatabase() {
    DatabaseTalking::TalkAndCheck("AT+SAPBR=1,1\r", 500, "OK");
    DatabaseTalking::TalkAndCheck("AT+HTTPINIT\r", 500, "OK");
    DatabaseTalking::TalkAndCheck("AT+HTTPPARA=CID,1\r", 500, "OK");
    DatabaseTalking::TalkAndCheck("AT+HTTPPARA=URL,\"http://car-protector.herokuapp.com/getLimits\"\r", 500, "OK");
    DatabaseTalking::TalkAndCheck("AT+HTTPDATA=192,5000\r", 1000, "OK");
    DatabaseTalking::TalkAndCheck("AT+HTTPACTION=0\r", 5000, "OK");
    int len = DatabaseTalking::TalkAndCheck("AT+HTTPREAD\r", 3000, "OK");

    char response[len];
    for (int i = 0; i < len; i++) {
        response[i] = buffer[i];
    }

    DatabaseTalking::TalkAndCheck("AT+HTTPTERM\r", 500, "OK");
    DatabaseTalking::TalkAndCheck("AT+SAPBR=0,1\r", 500, "OK");

    return response;
}

int DatabaseTalking::SendDataToDatabase() {return 0;}
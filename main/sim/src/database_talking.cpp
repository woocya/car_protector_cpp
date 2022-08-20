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
    DatabaseTalking::TalkAndCheck("AT+HTTPPARA=URL,\"http://testserver.aeq-web.com/sim800_test/sim800.php\"\r", 500, "OK");
    DatabaseTalking::TalkAndCheck("AT+HTTPPARA=CONTENT,application/x-www-form-urlencoded\r", 500, "OK");
    DatabaseTalking::TalkAndCheck("AT+HTTPDATA=192,5000\r", 1000, "OK");
    DatabaseTalking::TalkAndCheck("param=TestFromMySim800", 5000, "OK");
    DatabaseTalking::TalkAndCheck("AT+HTTPACTION=1\r", 5000, "OK");
    DatabaseTalking::TalkAndCheck("AT+HTTPREAD\r", 500, "OK");
    DatabaseTalking::TalkAndCheck("AT+HTTPTERM\r", 500, "OK");
    DatabaseTalking::TalkAndCheck("AT+SAPBR=0,1\r", 500, "OK");
}

int DatabaseTalking::GetDataFromDatabase() {return 0;}

int DatabaseTalking::SendDataToDatabase() {return 0;}
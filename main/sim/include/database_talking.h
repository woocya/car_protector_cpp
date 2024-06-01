#ifndef DATABASE_TALKING_H_
#define DATABASE_TALKING_H_

#include "uart_talk.h"

#include <iostream>
#include <sstream>
#include <stdlib.h>

class DatabaseTalking: public UartTalk {
private:

public:
    DatabaseTalking(int where);

    void activateGPRS();
    char * getDataFromDatabase();
    void sendDataToDatabase(const char * url);//double lat, double lon
};

#endif // DATABASE_TALKING_H_
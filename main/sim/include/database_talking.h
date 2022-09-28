#ifndef DATABASE_TALKING_H_
#define DATABASE_TALKING_H_

#include <iostream>
#include <sstream>
#include <stdlib.h>

#include "uart_talk.h"

class DatabaseTalking: public UartTalk {
private:

public:
    DatabaseTalking(int where);

    void ActivateGPRS();

    char * GetDataFromDatabase();

    void SendDataToDatabase(const char * url);//double lat, double lon
};

#endif // DATABASE_TALKING_H_
#ifndef DATABASE_TALKING_H_
#define DATABASE_TALKING_H_

#include <iostream>
#include <stdlib.h>

#include "uart_talk.h"

class DatabaseTalking: public UartTalk {
private:

public:
    DatabaseTalking(int where);

    void ActivateGPRS();

    int GetRequestFromCar();

    void SendRequestToUser(); //double lat, double lon

    int GetDataFromDatabase();

    int SendDataToDatabase();
};

#endif // DATABASE_TALKING_H_
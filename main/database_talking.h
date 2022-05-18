#ifndef DATABASE_TALKING_H_
#define DATABASE_TALKING_H_

#include <iostream>
#include <stdlib.h>

class DatabaseTalking {
private:

public:
    int GetRequestFromCar();

    int SendRequestToCar();

    int GetDataFromDatabase();

    int SendDataToDatabase();
};

#endif // DATABASE_TALKING_H_
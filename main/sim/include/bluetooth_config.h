#ifndef BLUETOOTH_CONFIG_H_
#define BLUETOOTH_CONFIG_H_

#include <iostream>
#include <stdlib.h>

#include "uart_talk.h"

class BluetoothConfig: public UartTalk {
private:

public:
    BluetoothConfig(int where);

    int Configure();

    int ParseScanning(int scan_length);

    int SendCommand(const char * command, int len_of_command);

    int ParseResponse(const char * expected_response);
};

#endif // BLUETOOTH_CONFIG_H_
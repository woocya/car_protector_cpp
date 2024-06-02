#ifndef BLUETOOTH_CONFIG_H_
#define BLUETOOTH_CONFIG_H_

#include "uart_talk.h"

#include <iostream>
#include <stdlib.h>

class BluetoothConfig: public UartTalk {
private:

public:
    BluetoothConfig(int where);

    int configure();
    int parseResponse(const char * expected_response);
    int parseScanning(int scan_length);
    std::pair<int, uint8_t*> sendCommand(const char * command, int len_of_command, int wait_for);
};

#endif // BLUETOOTH_CONFIG_H_
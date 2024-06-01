#include "bluetooth_config.h"

BluetoothConfig::BluetoothConfig(int where): UartTalk(where) {}

int BluetoothConfig::configure() {
    uartConversation("AT+BTPOWER=1\r\n", 1000);
    uartConversation("AT+BTUNPAIR=1\r\n", 3000);
    int scan_length = uartConversation("AT+BTSCAN=1,10\r\n", 10000);

    int num_of_device = parseScanning(scan_length);

    char * ch = (char *)malloc(13 + sizeof(int));
    sprintf(ch, "AT+BTPAIR=0,%d\r", num_of_device);
    uartConversation(ch, 3000);

    uartConversation("AT+BTPAIR=2,1234\r\n", 3000);
    uartConversation("AT+BTCONNECT=1,4\r", 3000);

    free(ch);
    return num_of_device;
}

int BluetoothConfig::parseScanning(int scan_length) {
    int i = 0; 
    int num_of_device;

    while(i < scan_length-1) {
        if ((const char)buffer[i] == ':' && (const char)buffer[i+1] == ' ') { // && scan_length < i + 11
            num_of_device = buffer[i+4] - '0';
            if ((const char)buffer[i+7] == 'O' && (const char)buffer[i+8] == 'B' && (const char)buffer[i+9] == 'D' && (const char)buffer[i+10] == 'I' && (const char)buffer[i+11] == 'I') {
                return num_of_device;
            }
            else {
                i += 11;
            }
        }
        else {
            i++;
        }
    }
    return -1;
}

std::pair<int, uint8_t*> BluetoothConfig::sendCommand(const char * command, int len_of_command, int wait_for) {
    char * fullChar = (char *)malloc(15 + sizeof(int));
    sprintf(fullChar, "AT+BTSPPSEND=%d\r\n", len_of_command);
    uartConversation(fullChar, 1000);

    int len_of_response = uartConversation(command, wait_for);

    free(fullChar);
    return std::pair(len_of_response, buffer);
}
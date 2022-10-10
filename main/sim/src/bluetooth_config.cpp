#include "bluetooth_config.h"

BluetoothConfig::BluetoothConfig(int where): UartTalk(where) {}

int BluetoothConfig::Configure() {
    UartConversation("AT+BTPOWER=1\r\n", 1000); //UART_SIM_PORT_NUM
    UartConversation("AT+BTUNPAIR=1\r\n", 3000);
    int scan_length = UartConversation("AT+BTSCAN=1,10\r\n", 10000);

    int num_of_device = ParseScanning(scan_length);

    char * ch = (char *)malloc(13 + sizeof(int));
    sprintf(ch, "AT+BTPAIR=0,%d\r", num_of_device);
    UartConversation(ch, 3000);

    UartConversation("AT+BTPAIR=2,1234\r\n", 3000);
    UartConversation("AT+BTCONNECT=1,4\r", 3000);

    free(ch);
    return num_of_device;
}

int BluetoothConfig::ParseScanning(int scan_length) {
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

std::pair<int, uint8_t*> BluetoothConfig::SendCommand(const char * command, int len_of_command, int wait_for) {
    char * fullChar = (char *)malloc(15 + sizeof(int));
    sprintf(fullChar, "AT+BTSPPSEND=%d\r\n", len_of_command);
    UartConversation(fullChar, 1000);

    int len_of_response = UartConversation(command, wait_for);

    free(fullChar);
    return std::pair(len_of_response, buffer);
}
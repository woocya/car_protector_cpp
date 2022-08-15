#include "bluetooth_config.h"

BluetoothConfig::BluetoothConfig(int where): UartTalk(where) {}

int BluetoothConfig::Configure() {
    UartConversation("AT+BTPOWER=1\r\n", 500); //UART_SIM_PORT_NUM
    UartConversation("AT+BTUNPAIR=0\r\n", 500);
    int scan_length = UartConversation("AT+BTSCAN=1,10\r\n", 500);

    int num_of_device = ParseScanning(scan_length);

    char * ch = (char *)malloc(13 + sizeof(int));
    sprintf(ch, "AT+BTPAIR=0,%d\r", num_of_device);
    UartConversation(ch, 1000);

    UartConversation("AT+BTPAIR=2,1234\r\n", 1000);

    ch = (char *)realloc(ch, 18 + sizeof(int));
    sprintf(ch, "AT+BTCONNECT=%d,4\r", num_of_device);
    UartConversation(ch, 1000);

    return num_of_device;
}

int BluetoothConfig::ParseScanning(int scan_length) {
    int i = 0; 
    int num_of_device;

    while(i < scan_length-1) {
        if ((const char)buffer[i] == ':' && (const char)buffer[i+1] == ' ' && scan_length < i + 11) {
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

int BluetoothConfig::SendCommand(const char * command, int len_of_command) {
    char * fullChar = (char *)malloc(len_of_command);
    sprintf(fullChar, "AT+BTSPPSEND=%d/r/n", 19);
    UartConversation(fullChar, 500);

    int len_of_response = UartConversation(command, 5000);
    return len_of_response;
}
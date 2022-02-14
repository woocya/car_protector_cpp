#ifndef PID_H_
#define PID_H_

#include <stdlib.h>
#include <iostream>

class PId {
private:
    int expected_bytes; //number of expected bytes in the response to this command
    const char* pid_command; 
    const char* parameter_name;
public:
    PId(int expected_bytes, const char* pid_message, const char* parameter_name);
    int GetPidExpectedBytes();
    const char* GetPidCommand();
    const char* GetParameterName();
};

static inline PId pid_array[5] 
{
    {PId(4, "0x01000D", "pids supported")},
    {PId(2, "0x010C0D", "engine speed")},
    {PId(1, "0x010D0D", "vehicle speed")},
    {PId(2, "0x011F0D", "run time since engine start")},
    {PId(1, "0x012F0D", "fuel tank level input")}
};

#endif PID_H_
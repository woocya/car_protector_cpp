#include <stdlib.h>
#include <iostream>

class PId {
private:
    int pid_hex;
    const char* pid_string;
    const char* parameterName;
public:
    PId(int pid_hex, const char* pid_string, const char* parameterName);
    int get_pid_hex();
    const char* get_pid_string();
    const char* get_parameterName();
};

static inline PId pidArray[5] 
{
    {PId(0x0100, "", "pids supported")},
    {PId(0x010C, "", "engine speed")},
    {PId(0x010D, "", "vehicle speed")},
    {PId(0x011F, "", "run time since engine start")},
    {PId(0x012F, "", "fuel tank level input")}
};

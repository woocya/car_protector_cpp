#include <stdlib.h>
#include <iostream>

class PId {
private:
    const char* pid_message;
    const char* parameterName;
public:
    PId(const char* pid_message, const char* parameterName);
    const char* get_pid_message();
    const char* get_parameterName();
};

static inline PId pidArray[5] 
{
    {PId("0x01000D", "pids supported")},
    {PId("0x010C0D", "engine speed")},
    {PId("0x010D0D", "vehicle speed")},
    {PId("0x011F0D", "run time since engine start")},
    {PId("0x012F0D", "fuel tank level input")}
};

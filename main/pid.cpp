#include "pid.h"

PId::PId(int expected_bytes, const char* pid_command, const char* parameter_name) {
    this->expected_bytes = expected_bytes;
    this->pid_command = pid_command;
    this->parameter_name = parameter_name;
}
int PId::GetPidExpectedBytes() {
    return expected_bytes;
}

const char* PId::GetPidCommand() {
    return pid_command;
}

const char* PId::GetParameterName() {
    return parameter_name;
}


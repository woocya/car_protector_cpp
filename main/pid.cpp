#include "pid.h"

PId::PId(int pid_hex, const char* pid_string, const char* parameterName) {
    this->pid_hex = pid_hex;
    this->pid_string = pid_string;
    this->parameterName = parameterName;
}

int PId::get_pid_hex() {
    return pid_hex;
}

const char* PId::get_pid_string() {
    return pid_string;
}

const char* PId::get_parameterName() {
    return parameterName;
}


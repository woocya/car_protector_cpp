#include "pid.h"

PId::PId(const char* pid_message, const char* parameterName) {
    this->pid_message = pid_message;
    this->parameterName = parameterName;
}

const char* PId::get_pid_message() {
    return pid_message;
}

const char* PId::get_parameterName() {
    return parameterName;
}


// #ifndef PID_H_
// #define PID_H_

// #include <stdlib.h>
// #include <iostream>

// static unsigned char checked_pids[] = {0b0000'0001, 0b0000'0010, 0b0000'0100, 0b0000'1000}; // engine_speed_active, vehicle_speed_active, runtime_active, fuel_tank_level_active
// static unsigned char active_pids[] = {0b0000'0000, 0b0000'0000, 0b0000'0000, 0b0000'0000};

// static inline PId pid_car_commands[6]  //still don't know which container to use for smallest possible occupation of memory
// {
//     {PId(4, "0x01000D", "pids supported 1")},
//     {PId(4, "0x01200D", "pids supported 2")},
//     {PId(2, "0x010C0D", "engine speed")},
//     {PId(1, "0x010D0D", "vehicle speed")}, //unclear if will work cause two "/r/r" and "/r" is end of command sign for obd
//     {PId(2, "0x011F0D", "run time since engine start")}, //doesnt work
//     {PId(1, "0x012F0D", "fuel tank level input")} //doesnt work
// };

// static inline PId pid_obd_commands[3] 
// {
//     {PId(12, "ATZ0x0D", "get device name")},
//     {PId(0, "ATSP30x0D", "set protocol to SAE J1850 VPW (10.4 kbaud)")}, //sp3 dziala + 38400
//     {PId(0, "ATE00x0D", "turn echo off")}
// };

// class PId {
// private:
//     int expected_bytes; //number of expected bytes in the response to this command
//     const char* pid_command; 
//     std::string parameter_name;
// public:
//     PId(int expected_bytes, const char* pid_message, std::string parameter_name);
//     int GetPidExpectedBytes();
//     const char* GetPidCommand();
//     std::string GetParameterName();
// };

// //deprecated

// #endif PID_H_
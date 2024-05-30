idf_component_register(
    SRCS "car/src/car_talking.cpp" "car/src/motion_sensor.cpp" "car/src/values.cpp" "sim/src/bluetooth_config.cpp" "sim/src/database_talking.cpp" "sim/src/gps_data_parsing.cpp" "sim/src/sim_sms.cpp" "uart/src/uart_config.cpp" "uart/src/uart_talk.cpp" "wifi/src/config_wifi.cpp" "wifi/src/http_request.cpp" "wifi/src/main_wifi.cpp" "wifi/src/wifi.cpp" "main.cpp" "main_final.cpp" "pid.cpp"
    INCLUDE_DIRS "car/include" "sim/include" "uart/include" "wifi/include" 
)

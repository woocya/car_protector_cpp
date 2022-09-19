idf_component_register(
    SRCS "main_final.cpp" "wifi/src/wifi.cpp" "wifi/src/mainwifi.cpp" "wifi/src/http_request.cpp" "sim/src/sim_sms.cpp" "uart/src/uart_talk.cpp" "uart/src/uart_config.cpp" "sim/src/gps_data_parsing.cpp" "sim/src/bluetooth_config.cpp" "pid.cpp" "main.cpp" "sim/src/database_talking.cpp" "car/src/car_talking.cpp"
    INCLUDE_DIRS "wifi/include" "uart/include" "sim/include" "car/include"
)

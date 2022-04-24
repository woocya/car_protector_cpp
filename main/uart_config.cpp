#include "uart_config.h"

static void uart_config()
{
    /* Configure parameters of an UART driver,
     * communication pins and install the driver */
    uart_config_t uart_sim_config = {
        .baud_rate = UART_SIM_BAUD_RATE,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_APB,
    };

    uart_config_t uart_obd_config = {
        .baud_rate = UART_OBD_BAUD_RATE,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_APB,
    };
    int intr_alloc_flags = 0;

    ESP_ERROR_CHECK(uart_driver_install(UART_OBD_PORT_NUM, UART_BUF_SIZE * 2, 0, 0, NULL, intr_alloc_flags));
    ESP_ERROR_CHECK(uart_param_config(UART_OBD_PORT_NUM, &uart_obd_config));
    ESP_ERROR_CHECK(uart_set_pin(UART_OBD_PORT_NUM, UART_OBD_TXD, UART_OBD_RXD, UART_RTS, UART_CTS));

    ESP_ERROR_CHECK(uart_driver_install(UART_SIM_PORT_NUM, UART_BUF_SIZE * 2, 0, 0, NULL, intr_alloc_flags));
    ESP_ERROR_CHECK(uart_param_config(UART_SIM_PORT_NUM, &uart_sim_config));
    ESP_ERROR_CHECK(uart_set_pin(UART_SIM_PORT_NUM, UART_SIM_TXD, UART_SIM_RXD, UART_RTS, UART_CTS));

}

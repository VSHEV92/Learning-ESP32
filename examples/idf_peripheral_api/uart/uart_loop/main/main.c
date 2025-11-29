#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/uart.h"
#include "string.h"

#define UART_BUF_SIZE 1024
#define UART_TXD_PIN 0
#define UART_RXD_PIN 1
#define UART_BAUD_RATE 115200
#define UART_SEND_DELAY_MS 1000

static void tx_task(void *arg);
static void rx_task(void *arg);

const char uart_data[] = "Hello from TX task!";

void app_main(void) {

    // initialize uart driver
    uart_driver_install(
        UART_NUM_1,      // UART port
        UART_BUF_SIZE,   // RX buffer size in bytes 
        0,               /* TX buffer size in bytes.  
                            If set to zero, driver will not use TX buffer, 
                            TX function will block task until all data have been sent out
                         */
        0,               // event queue size. used in callback handled 
        NULL,            /* event queue, created by the driver. 
                            Used in callback handled
                            If set to NULL, driver will not use an event queue.
                        */
        0               // interrupt allocation flags
    );
    

    // configure uart 
    uart_config_t uart_config = {
        .baud_rate = UART_BAUD_RATE,            // uart baud rate
        .data_bits = UART_DATA_8_BITS,          // number of data bits (5,6,7,8)
        .parity = UART_PARITY_DISABLE,          // enable or disable parity bit
        .stop_bits = UART_STOP_BITS_1,          // number of stop bits (1, 1.5, 2) 
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,  // HW flow control configuration (disable, RTS, CTS, RTS and CTS)
        .rx_flow_ctrl_thresh = 0,               // HW RTS threshold, if flow control not disabled. Not used so set to 0
        .source_clk = UART_SCLK_DEFAULT,        // uart port clock source
        .flags.backup_before_sleep = false,     // If set, the driver will backup/restore UART registers after sleep mode
    };
    uart_param_config(UART_NUM_1, &uart_config);


    // set uart pins
    uart_set_pin(
        UART_NUM_1,          // UART port
        UART_TXD_PIN,        // UART TX pin number
        UART_RXD_PIN,        // UART RX pin number
        UART_PIN_NO_CHANGE,  // UART RTS pin number, set last value 
        UART_PIN_NO_CHANGE   // UART CTS pin number, set last value
    );
    

    // create UART RX task
    xTaskCreate(
        rx_task, 
        "uart_rx_task", 
        4096, 
        NULL, 
        2, 
        NULL
    );
    

    // create UART TX task
    xTaskCreate(
        tx_task, 
        "uart_tx_task", 
        4096, 
        NULL, 
        1, 
        NULL
    );
}

// ----------------------------------
// --------- UART TX Task -----------
void tx_task(void *arg) {
    const int len = strlen(uart_data);

    while (true) {

        int sended_bytes = uart_write_bytes(
            UART_NUM_1, // UART port 
            uart_data,  // tx data buffer
            len + 1     // bytes to send, string len + NULL character
        );

        ESP_LOGI("TX Task", "Send %d bytes", sended_bytes); 
        vTaskDelay( pdMS_TO_TICKS(UART_SEND_DELAY_MS) );
    }
}

// ----------------------------------
// --------- UART RX Task -----------
void rx_task(void *arg) {
    uint8_t rx_data[UART_BUF_SIZE]; 
    const int len = strlen(uart_data);


    while (true) {
        
        int readed_bytes = uart_read_bytes(
            UART_NUM_1,                             // UART port
            rx_data,                                // rx data buffer
            len + 1,                                // bytes to read, string len + NULL character
            pdMS_TO_TICKS(UART_SEND_DELAY_MS * 2)   // read timeout in ticks 
        );
        
        ESP_LOGI("RX Task", "Read %d bytes: '%s'", readed_bytes, rx_data);
    }
}

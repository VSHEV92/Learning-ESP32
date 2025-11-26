#include "esp_log.h"
#include "driver/i2c_master.h"

static const char *TAG = "bmp180_probe";

#define BMP180_SCL       1
#define BMP180_SDA       0 
#define BMP180_ADDR      0x77 
#define BMP180_FREQ_HZ   100000 // 100kHz


void app_main(void) {

    // configure i2c master bus 
    i2c_master_bus_config_t i2c_bus_master_cfg = {
        .i2c_port = -1,                // I2C port number, `-1` for auto selecting, 0 only valid for esp32c3 
        .sda_io_num = BMP180_SDA,             // GPIO number of I2C SDA
        .scl_io_num = BMP180_SCL,             // GPIO number of I2C SCL
        .clk_source = I2C_CLK_SRC_DEFAULT,    // Clock source of I2C master bus
        .glitch_ignore_cnt = 7,               // SCL, SDA, filter glitch period, 7 is default value
        .intr_priority = 0,                   // I2C interrupt priority, 0 - default priority (1,2,3)
        .trans_queue_depth = 0,               // Transaction queue, only for async transfers
        .flags.enable_internal_pullup = true  // Enable internal pullups
    };

    i2c_master_bus_handle_t i2c_bus_master_handle;
    i2c_new_master_bus(&i2c_bus_master_cfg, &i2c_bus_master_handle);


    // configure bmp180 i2c device
    i2c_device_config_t bmp180_cfg = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,  // Address length of the slave device
        .device_address = BMP180_ADDR,          // I2C device raw address
        .scl_speed_hz = BMP180_FREQ_HZ,         // SCL line frequency in Hz
        .scl_wait_us = 0,                       // Timeout value of SCL line idle (unit: us). 0 - default reg value
        .flags.disable_ack_check = false        // Disable ACK check
    };

    i2c_master_dev_handle_t bmp180_handle;
    i2c_master_bus_add_device(i2c_bus_master_handle, &bmp180_cfg, &bmp180_handle);


    // probe bmp180 device
    ESP_LOGI(TAG, "Try probe BMP180 device");

    esp_err_t probe_status = i2c_master_probe(
        i2c_bus_master_handle, // i2c bus master handle 
        BMP180_ADDR,           // i2c device slave address
        -1                     // timeout in ms, -1 - wait forever
    );

    if (probe_status == ESP_OK) {
        ESP_LOGI(TAG, "BMP180 is probed");
    } 
    else {
        ESP_LOGI(TAG, "No BMP180 device on the bus");
    }
    
    
    // read bmp180 device ID
    ESP_LOGI(TAG, "Try to read BMP180 device ID");

    uint8_t tx_buf[1] = {0xD0};
    uint8_t rx_buf[1];

    i2c_master_transmit_receive(
        bmp180_handle,  // i2c device handle 
        tx_buf,         // TX buffer 
        1,              // TX buffer size
        rx_buf,         // RX buffer 
        1,              // RX buffer size
        -1              // timeout in ms, -1 - wait forever
    );

    ESP_LOGI(TAG, "BMP180 device ID is %x", (int)rx_buf[0]);
}

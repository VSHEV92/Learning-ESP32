#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "bmp180/bmp180.h"

static const char *TAG = "bmp180_measure";

#define BMP180_SCL               1
#define BMP180_SDA               0 
#define BMP180_MEASURE_PERIOD_MS 1000

void app_main(void) {
    // configure bmp180 device
    i2c_lowlevel_config bmp180_cfg = {
        .port = I2C_NUM_0,
        .pin_sda = BMP180_SDA,
        .pin_scl = BMP180_SCL,
    };
    
    bmp180_t* bmp180_device = bmp180_init(
        &bmp180_cfg,                 // bmp180 configuration
        0,                           // bmp180 device address, 0 - default address 0x77
        BMP180_MODE_HIGH_RESOLUTION  // OSS - oversampling setting, set to 13.5ms convertion time
    );
   
    // measure temperature and pressure
    float temperature;
    uint32_t pressure; 
    while(true) {
        bmp180_measure(bmp180_device, &temperature, &pressure);
        ESP_LOGI(TAG, "Temperature: %.2f C, Pressure: %u Pa", temperature, (unsigned int)pressure);
        vTaskDelay( pdMS_TO_TICKS(BMP180_MEASURE_PERIOD_MS) );
    }
}

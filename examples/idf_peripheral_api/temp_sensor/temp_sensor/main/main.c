#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/temperature_sensor.h"
#include "esp_log.h"

#define TEMP_MESUREMENTS_PERIOD_MS 500

static const char *TAG = "temp_sensor";

void app_main(void)
{
    // configure temperature sensor
    temperature_sensor_config_t temp_sensor_cfg = TEMPERATURE_SENSOR_CONFIG_DEFAULT(10, 50);

    // init temperature sensor handle
    temperature_sensor_handle_t temp_sensor;
    temperature_sensor_install(&temp_sensor_cfg, &temp_sensor);

    // enable temperature sensor
    temperature_sensor_enable(temp_sensor);

    // get tamperature and print it on the screen
    float temperature; 
    while (true) {
        temperature_sensor_get_celsius(temp_sensor, &temperature);
        ESP_LOGI(TAG, "Temperature: %.02f ℃", temperature);
        vTaskDelay( pdMS_TO_TICKS(TEMP_MESUREMENTS_PERIOD_MS) );
    }
}

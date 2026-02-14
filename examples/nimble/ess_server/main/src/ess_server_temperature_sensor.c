#include "ess_server_temperature_sensor.h"

temperature_sensor_handle_t ess_server_temperature_sensor_instance;

void ess_server_temperature_sensor_init(){
    // configure temperature sensor
    temperature_sensor_config_t temp_sensor_cfg = TEMPERATURE_SENSOR_CONFIG_DEFAULT(10, 50);

    // init temperature sensor handle
    temperature_sensor_install(&temp_sensor_cfg, &ess_server_temperature_sensor_instance);

    // enable temperature sensor
    temperature_sensor_enable(ess_server_temperature_sensor_instance);
}

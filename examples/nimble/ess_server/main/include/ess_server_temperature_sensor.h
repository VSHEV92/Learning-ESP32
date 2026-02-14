#ifndef ESS_SERVER_TEMPERATURE_SENSOR_H
#define ESS_SERVER_TEMPERATURE_SENSOR_H

#include "driver/temperature_sensor.h"

extern temperature_sensor_handle_t ess_server_temperature_sensor_instance;

void ess_server_temperature_sensor_init();

#endif

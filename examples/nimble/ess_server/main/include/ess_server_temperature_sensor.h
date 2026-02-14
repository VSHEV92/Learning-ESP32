#ifndef ESS_SERVER_TEMPERATURE_SENSOR_H
#define ESS_SERVER_TEMPERATURE_SENSOR_H

#include "driver/temperature_sensor.h"

void ess_server_temperature_sensor_init();
float ess_server_temperature_sensor_get_temp();

#endif

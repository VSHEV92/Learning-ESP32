#ifndef ESS_CLIENT_LED_MATRIX_H
#define ESS_CLIENT_LED_MATRIX_H

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"

#include "esp_log.h"

#include "nimble/nimble_port.h"
#include "host/ble_hs.h"
#include "services/gatt/ble_svc_gatt.h"

#include "max7219.h"


#define MAX7219_SCLK 0
#define MAX7219_MOSI 2
#define MAX7219_CS 3

void ess_client_led_matrix_init(void);
void ess_client_led_matrix_start_read(uint16_t conn_handle, uint16_t att_handle);
void ess_client_led_matrix_stop_read();

#endif

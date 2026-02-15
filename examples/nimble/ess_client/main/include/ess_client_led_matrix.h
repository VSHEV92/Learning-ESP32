#ifndef ESS_CLIENT_LED_MATRIX_H
#define ESS_CLIENT_LED_MATRIX_H

#include "esp_log.h"

#include "nimble/nimble_port.h"
#include "host/ble_hs.h"
#include "services/gatt/ble_svc_gatt.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"

void ess_client_led_matrix_init(void);
void ess_client_led_matrix_start_read(uint16_t conn_handle, uint16_t att_handle);
void ess_client_led_matrix_stop_read();

#endif

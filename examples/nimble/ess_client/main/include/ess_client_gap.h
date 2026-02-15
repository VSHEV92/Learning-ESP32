#ifndef ESS_CLIENT_GAP_H
#define ESS_CLIENT_GAP_H

#include "esp_log.h"

#include "nimble/nimble_port.h"
#include "host/ble_hs.h"
#include "services/gap/ble_svc_gap.h"
#include "services/gatt/ble_svc_gatt.h"

#define ESS_CLIENT_GAP_UUID_ESS 0x181A
#define ESS_CLIENT_GAP_UUID_TEMP_CELSIUS 0x272F

#define ESS_CLIENT_GAP_DEVICE_NAME "esp32_ess_client"
#define ESS_CLIENT_GAP_DEVICE_APPEARANCE 0x0180

void ess_client_gap_init(void);
void ess_client_gap_start_discovery(void);

#endif

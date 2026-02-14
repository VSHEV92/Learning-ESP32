#ifndef ESS_SERVER_GAP_H
#define ESS_SERVER_GAP_H

#include "nimble/nimble_port.h"
#include "host/ble_hs.h"
#include "services/gap/ble_svc_gap.h"

#define ESS_SERVER_GAP_DEVICE_NAME "esp32_ess_example"
#define ESS_SERVER_GAP_DEVICE_APPEARANCE 0x0300

void ess_server_gap_init(void);
void ess_server_gap_start_advertising(void);

#endif

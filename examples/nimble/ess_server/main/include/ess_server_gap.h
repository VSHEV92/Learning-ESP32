#ifndef ESS_SERVER_GAP_H
#define ESS_SERVER_GAP_H

#include "esp_log.h"

#include "nimble/nimble_port.h"
#include "host/ble_hs.h"
#include "services/gap/ble_svc_gap.h"

#define ESS_SERVER_GAP_DEVICE_NAME "esp32_ess_example"
#define ESS_SERVER_GAP_DEVICE_APPEARANCE 0x0300
#define ESS_SERVER_GAP_LE_ROLE_PERIPHERAL 0x00

void ess_server_gap_init(void);
void ess_server_gap_start_advertising(void);
int ess_server_gap_event_handler(struct ble_gap_event *event, void *arg);

#endif

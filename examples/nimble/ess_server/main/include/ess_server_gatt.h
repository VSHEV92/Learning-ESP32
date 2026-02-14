#ifndef ESS_SERVER_GATT_H
#define ESS_SERVER_GATT_H

#include "nimble/nimble_port.h"
#include "host/ble_hs.h"
#include "services/gatt/ble_svc_gatt.h"

#define ESS_SERVER_GATT_UUID_ESS                0x181A

#define ESS_SERVER_GATT_UUID_TEMP_CELSIUS       0x272F

#define ESS_SERVER_GATT_UUID_ES_MESUAREMENT     0x290C
#define ESS_SERVER_GATT_UUID_ES_TRIGGER_SETTING 0x290D

#define ESS_SERVER_GATT_UUID_USER_DESCRIPTION   0x2901

void ess_server_gatt_init(void);

#endif

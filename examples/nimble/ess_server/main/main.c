#include "ess_server.h"

void app_main(void) {
    // Temperature sensor initialize
    ess_server_temperature_sensor_init();
    
    // NimBLE host stack initialization
    nimble_port_init();

    // GAP service initialization
    ess_server_gap_init();

    // GATT service initialization
    ess_server_gatt_init();

    // Start HCI events processing
    nimble_port_run();
}

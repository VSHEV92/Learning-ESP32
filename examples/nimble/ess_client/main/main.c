#include "ess_client.h"

void app_main(void) {
    // NimBLE host stack initialization
    nimble_port_init();

    // Led matrix initialization
    ess_client_led_matrix_init();

    // GAP service initialization
    ess_client_gap_init();

    // Start HCI events processing
    nimble_port_run();
}

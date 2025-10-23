#include <custom_init.h>

// prototypes of esp32c3 rom functions
int ets_printf(const char *format, ...);

void __attribute__((noreturn)) main() {
    
    // run custom soc init
    custom_init();

    ets_printf("Hello from custom freertos port!\n");
    
    while(1) {}

}

#include <custom_init.h>

// prototypes of esp32c3 rom functions
int ets_printf(const char *format, ...);
void ets_delay_us(uint32_t us);

void __attribute__((noreturn)) main() {
    
    // run custom soc init
    custom_init();

    while(1) {
        ets_printf("Hello from custom CMake build!\n");
        
        // esp_rom think that cpu has default clock frequency 20 MHz
        // but custom_init configure cpu clock to 160 MHz
        ets_delay_us(1000000 * (160/20));
    }

}

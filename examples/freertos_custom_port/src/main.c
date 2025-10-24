#include <custom_init.h>
#include <FreeRTOS.h>
#include <task.h>

void __attribute__((noreturn)) main() {
    
    // run custom soc init
    custom_init();

    vTaskStartScheduler();
    
    for( ;; );
    

}

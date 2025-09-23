#include "stdio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

/*-- portmacro.h --
    portTICK_PERIOD_MS - period of one tick in milliseconds. 
                         Define using configTICK_RATE_HZ configuration
*/

/*-- projdefs.h --
    pdMS_TO_TICKS - convert time period in milliseconds to number of scheduler ticks.  
                    Define using configTICK_RATE_HZ configuration
*/

/*-- task.h --
    xTaskGetTickCount - return number of ticks since scheduler started
   
    vTaskDelay - delay task for a given number of ticks. 
                 Delay is relative the time when function is called 

    xTaskDelayUntil - delay task until given tick number. 
                      Delay to is absolute tick number.
                      Good for create periodic functions
                      Return pdFalse if next wakeup time is in the past

    vTaskDelayUntil - same as xTaskDelayUntil but not return any value
*/
void app_main(void) {
    
    TickType_t delay_until_last_wakeup;
    printf("Start app!\n");

    while (true) {

        // first 1 second delay using ralative tick value
        vTaskDelay( pdMS_TO_TICKS(1000) );
        printf("After first 1 second delay!\n");

        // second to second delay using absolute tick value
        delay_until_last_wakeup = xTaskGetTickCount();
        vTaskDelayUntil(&delay_until_last_wakeup, pdMS_TO_TICKS(2000) );
        printf("After second 2 seconds delay!\n");
    }
}

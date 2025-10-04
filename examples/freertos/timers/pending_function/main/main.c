#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"

static const char* TAG = "freertos_example";

void pending_function( void * param1, uint32_t param2 );

void app_main(void) {

    ESP_LOGI(TAG, "Send pending command function to timer deamon");

    xTimerPendFunctionCall(
        pending_function,           // function to call 
        (void*) "string_param1",    // first parameter
        42,                         // second parameter
        portMAX_DELAY               // delay for deamon command queue waiting
    );

    vTaskDelay( pdMS_TO_TICKS(1000) );
}

void pending_function( void * param1, uint32_t param2 ) {
    char* string_param = (char*)param1;
    unsigned int int_param = (unsigned int)param2;

    ESP_LOGI(TAG, "Hello from pending function. Param1 is %s. Param2 is %u", string_param, int_param);
}

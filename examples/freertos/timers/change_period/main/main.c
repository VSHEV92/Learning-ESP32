#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"

#define TIMER_PERIOD_MS 1000

static const char* TAG = "freertos_example";

void timer_callback( TimerHandle_t xTimer );

void app_main(void) {
    TimerHandle_t one_shot_timer;


    // create one shot timer
    ESP_LOGI(TAG, "Create timer");

    one_shot_timer = xTimerCreate (
        "one_shot",                         // human readable timer name
        pdMS_TO_TICKS( TIMER_PERIOD_MS ),   // timer period in ticks
        pdFALSE,                            // disable autoreload
        NULL,                               // set timer ID
        timer_callback                      // timer callback function 
    );

    // check that timer successfully created
    if (one_shot_timer == NULL) {
        ESP_LOGE(TAG, "Can't create timer!");
        abort();
    }

    // start timer
    ESP_LOGI(TAG, "Start timer");
    xTimerStart(
        one_shot_timer,     // timer handler
        portMAX_DELAY       // max delay for waiting until command send to deamon command queue
    );

    // wait until all timer is triggered
    vTaskDelay( pdMS_TO_TICKS(TIMER_PERIOD_MS * 2) );

    // double timer period. That cause to restart timer
    ESP_LOGI(TAG, "Change timer period");
    xTimerChangePeriod(
        one_shot_timer,                         // timer handler
        pdMS_TO_TICKS( TIMER_PERIOD_MS * 2 ),   // new timer period in ticks
        portMAX_DELAY                           // max delay for waiting until command send to deamon command queue
    );
    
    // wait until all timer is triggered
    vTaskDelay( pdMS_TO_TICKS(TIMER_PERIOD_MS * 3) );

    // delete timer
    ESP_LOGI(TAG, "Delete timers");
    xTimerDelete(
        one_shot_timer,     // timer handler
        portMAX_DELAY       // max delay for waiting until command send to deamon command queue
    );
    
}

void timer_callback( TimerHandle_t xTimer ) {
    ESP_LOGI(TAG, "Hello from timer");
}

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"

#define TIMER_NUMBER 5
#define TIMER_PERIOD_MS 1000
#define TIMER_PERIOD_INC_MS 533

static const char* TAG = "freertos_example";

static char timer_names[TIMER_NUMBER][40];

void timer_callback( TimerHandle_t xTimer );

void app_main(void) {
    TimerHandle_t autoreload_timer[TIMER_NUMBER];

    ESP_LOGI(TAG, "Create timers");

    for (int i = 0; i < TIMER_NUMBER; i++) {
        // set timer name
        sprintf(timer_names[i], "Oneshot timer %d", i);
        // create one shot timers
        autoreload_timer[i] = xTimerCreate (
            timer_names[i],                                                      // human readable timer name
            pdMS_TO_TICKS(TIMER_PERIOD_MS + TIMER_PERIOD_INC_MS * (i + 1) ),     // timer period in ticks
            pdTRUE,                                                              // enable autoreload
            ( void * ) 0,                                                        // set timer ID
            timer_callback                                                       // timer callback function 
        );

        // check that timer successfully created
        if (autoreload_timer[i] == NULL) {
            ESP_LOGE(TAG, "Can't create timer %d!", i);
            abort();
        }
    }

    ESP_LOGI(TAG, "Start timers");

    for (int i = 0; i < TIMER_NUMBER; i++) {
        // start timer
        xTimerStart(
            autoreload_timer[i],  // timer handler
            portMAX_DELAY         // max delay for waiting until command send to deamon command queue
        );
    }
}

void timer_callback( TimerHandle_t xTimer ) {
    // get timer name
    const char* timer_name = pcTimerGetName(xTimer);

    // get timer id
    int timer_id = ( int ) pvTimerGetTimerID( xTimer );

    // print timer message
    ESP_LOGI(TAG, "%s is triggered %d times", timer_name, timer_id);
    
    // inrement and store timer id
    vTimerSetTimerID(xTimer, (void*)++timer_id);
}

#include "ess_client_led_matrix.h"

static uint16_t ess_server_connection_handle = 0;
static uint16_t ess_server_chr_temp_celsius_handle = 0;

static TimerHandle_t temp_read_timer;
static TaskHandle_t led_martix_update_task_handle;

void ess_client_led_matrix_init(void);
void ess_client_led_matrix_start_read(uint16_t conn_handle, uint16_t att_handle);
void ess_client_led_matrix_stop_read();

static int temp_read_cb(uint16_t conn_handle, const struct ble_gatt_error *error, struct ble_gatt_attr *attr, void *arg);
static void temp_read_timer_callback( TimerHandle_t xTimer );
static void led_matrix_update_task(void* param);


void ess_client_led_matrix_init(void) {
    // create temperature read timer
    temp_read_timer = xTimerCreate (
        "temperature read",                             // human readable timer name
        pdMS_TO_TICKS( 1000 ),                          // timer period in ticks
        pdTRUE,                                         // enable autoreload
        ( void * ) 0,                                   // set timer ID
        temp_read_timer_callback                        // timer callback function 
    );

    // create and run led matrix update task
    xTaskCreate(
        led_matrix_update_task,
        "led_matrix_update_task",
        4096,
        NULL,
        1,
        &led_martix_update_task_handle    
    );
}


void ess_client_led_matrix_start_read(uint16_t conn_handle, uint16_t att_handle) {
    ess_server_connection_handle = conn_handle;
    ess_server_chr_temp_celsius_handle = att_handle;
    xTimerStart(temp_read_timer, portMAX_DELAY);
}


void ess_client_led_matrix_stop_read() {    
    xTimerStop(temp_read_timer, portMAX_DELAY);
}


static void temp_read_timer_callback( TimerHandle_t xTimer ) {
    ble_gattc_read(ess_server_connection_handle, ess_server_chr_temp_celsius_handle, temp_read_cb, NULL);
}


static int temp_read_cb(uint16_t conn_handle, const struct ble_gatt_error *error, struct ble_gatt_attr *attr, void *arg) {
    float temp_value;
    if (error->status == 0) {
        sscanf((char*)attr->om->om_data, "%f", &temp_value);
        xTaskNotify( 
            led_martix_update_task_handle,      // task handle 
            (uint32_t)temp_value,               // notify value
            eSetValueWithOverwrite              // action under notify value
        );
    }
    return 0;
}


static void led_matrix_update_task(void* param) {
    uint32_t temp_value;

    while(true){
        xTaskNotifyWait( 
            0x0,            // clear notify value under function entry
            0x0,            // clear notify value under function exit
            &temp_value,    // notify value
            portMAX_DELAY   // max notify wait delay
        );
        
        ESP_LOGI("", "Read ESS Temperature: %dC", temp_value);
    
    }
}

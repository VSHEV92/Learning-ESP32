#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "nimble/nimble_port.h"
#include "host/ble_hs.h"

#include "driver/gpio.h"
#include "esp_adc/adc_oneshot.h"


// ------------ defines ----------------
#define TASK_STACK_SIZE 4096

#define LED_COLOR_HANDLE 3
#define LED_BRIGHTNESS_HANDLE 5

#define BUTTON_GPIO 9
#define ESP_INTR_FLAG_DEFAULT 0

// ------------ globals ----------------
uint16_t connection_handle = 0;
bool connection_status = false;

TaskHandle_t button_task_handle = NULL; 
TaskHandle_t adc_task_handle = NULL; 

adc_oneshot_unit_handle_t adc_handle;

// ------------ functions ----------------

//extern const struct ble_gatt_svc_def gatt_svcs[];
extern int gap_event_handler(struct ble_gap_event *event, void *arg);

void button_task(void* param) {
    while (1) {
        ulTaskNotifyTake( 
            pdTRUE,         // clear value after get notification
            portMAX_DELAY   // max notify wait delay
        );

        if (connection_status) {
            ble_gattc_write_flat(
                connection_handle,  // coonnection handler 
                LED_COLOR_HANDLE,   // attribute handler          
                NULL,               // value address
                0,                  // value size
                NULL,               // completion callback
                NULL                // completion callback argument
            );
        }
    }
}


void adc_task(void* param) {
    int adc_raw_data;
    uint8_t led_brightness;

    while(1) {
        vTaskDelay( pdMS_TO_TICKS(200) );
        adc_oneshot_read(adc_handle, ADC_CHANNEL_0, &adc_raw_data);

        led_brightness = (adc_raw_data * 256) / 4096;

        if (connection_status) {
            ble_gattc_write_flat(
                connection_handle,       // coonnection handler 
                LED_BRIGHTNESS_HANDLE,   // attribute handler          
                &led_brightness,         // value address
                1,                       // value size
                NULL,                    // completion callback
                NULL                     // completion callback argument
            );
        }
    }
}


void gpio_button_handler(void* arg) {
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    vTaskNotifyGiveFromISR( button_task_handle, &xHigherPriorityTaskWoken );  
    portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
}

// NimBLE on_sync callback
void start_discovery(void) {

    struct ble_gap_disc_params disc_params = {
        .itvl = 0,                  // Scan interval in 0.625ms units
        .window = 0,                // Scan window in 0.625ms units
        .filter_policy = 0,         // Scan filter policy
        .limited = 0,               // If limited discovery procedure should be used
        .passive = 1,               // If passive scan should be used
        .filter_duplicates = 1,     // If enable duplicates filtering
        .disable_observer_mode = 0, // If observation procedure is to be disabled. By default all advertisement reports are accepted
    };

    ble_gap_disc (
        BLE_OWN_ADDR_PUBLIC,   // The type of address the stack should use for itself when sending scan requests
        BLE_HS_FOREVER,        // The duration of the discovery procedure. Units are milliseconds.  Specify BLE_HS_FOREVER for no expiration. Specify 0 to use stack defaults
        &disc_params,          // Additional arguments specifying the particulars of the discovery procedure    
        gap_event_handler,     // The callback to associate with this discovery procedure
        NULL                   // The optional argument to pass to the callback function
    );

}

void app_main(void) {

    // ----------------------------------------
    // --------- INIT GPIO BUTTON -------------
    // ----------------------------------------
    gpio_config_t io_conf = {};
    
    // configure button gpio
    io_conf.pin_bit_mask = 1 << BUTTON_GPIO;
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.intr_type = GPIO_INTR_NEGEDGE;
    io_conf.pull_down_en = 0;
    io_conf.pull_up_en = 1;
    gpio_config(&io_conf);

    // configure interrupt
    gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);
    gpio_isr_handler_add(BUTTON_GPIO, gpio_button_handler, (void*)NULL);

    // run button task
    xTaskCreate(
        button_task,
        "button_task",
        TASK_STACK_SIZE,
        NULL,
        1,
        &button_task_handle
    );

    // ----------------------------------------
    // -------------- INIT ADC ----------------
    // ----------------------------------------

    // configure ADC unit
    adc_oneshot_unit_init_cfg_t adc_unit_cfg = {
        .unit_id = ADC_UNIT_1,
        .clk_src = ADC_DIGI_CLK_SRC_APB,
        .ulp_mode = ADC_ULP_MODE_DISABLE
    };

    adc_oneshot_new_unit(&adc_unit_cfg, &adc_handle);

    // configure ADC channel
    adc_oneshot_chan_cfg_t adc_channel_cfg = {
        .bitwidth = ADC_BITWIDTH_DEFAULT, // max supported bitwidth
        .atten = ADC_ATTEN_DB_12,
    };
    adc_oneshot_config_channel(adc_handle, ADC_CHANNEL_0, &adc_channel_cfg);

    // run adc task
    xTaskCreate(
        adc_task,
        "adc_task",
        TASK_STACK_SIZE,
        NULL,
        1,
        &adc_task_handle
    );

    // ----------------------------------------
    // --------------- INIT BT ----------------
    // ----------------------------------------

    // Set NimBLE stack sync callback
    ble_hs_cfg.sync_cb = start_discovery;
    
    // NimBLE host stack initialization
    nimble_port_init();

    // run nimble stack
    nimble_port_run();

}

#include "ess_client_led_matrix.h"
#include "ess_client_led_matrix_digits.h"

static uint16_t ess_server_connection_handle = 0;
static uint16_t ess_server_chr_temp_celsius_handle = 0;

static TimerHandle_t temp_read_timer;
static TaskHandle_t led_martix_update_task_handle;

static max7219_t max7219_device =
{
    .cascade_size = 1, // number of max7219 devices connected in cacsade
    .digits = 0,       // number of digits in 7seg or lines in 8x8, 0 - maximum 8 digits 
    .mirrored = false, // mirror digits in 7seg or flip lines in 8x8 
    .bcd = false       // enable bcd 
};

void ess_client_led_matrix_init(void);
void ess_client_led_matrix_start_read(uint16_t conn_handle, uint16_t att_handle);
void ess_client_led_matrix_stop_read();

static int temp_read_cb(uint16_t conn_handle, const struct ble_gatt_error *error, struct ble_gatt_attr *attr, void *arg);
static void temp_read_timer_callback( TimerHandle_t xTimer );
static void led_matrix_update_task(void* param);


void ess_client_led_matrix_init(void) {
    // configure spi bus
    spi_bus_config_t spi_bus_cfg = {
        .sclk_io_num = MAX7219_SCLK,
        .mosi_io_num = MAX7219_MOSI,
        .miso_io_num = -1,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 2,
        .flags = 0
    };
    spi_bus_initialize(SPI2_HOST, &spi_bus_cfg, SPI_DMA_CH_AUTO);
    
    max7219_init_desc(
        &max7219_device,            // pointer to max7219_t structure
        SPI2_HOST,                  // spi host, SPI2_HOST is only valid host for esp32c3 
        MAX7219_MAX_CLOCK_SPEED_HZ, // spi clock rate in Hz, set to max clock rate of max7219 - 10 MHz 
        MAX7219_CS                  // spi CS pin number 
    );

    // inititalize max7219 device
    // 1. Disable test
    // 2. Set max scan limit
    // 3. Set normal decode mode 
    // 4. Clear screen
    // 5. Set minimal brightness
    max7219_init(&max7219_device);

    // enable shutdown mode
    max7219_set_shutdown_mode(&max7219_device, true);


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
    max7219_set_shutdown_mode(&max7219_device, false);
}


void ess_client_led_matrix_stop_read() {    
    xTimerStop(temp_read_timer, portMAX_DELAY);
    max7219_set_shutdown_mode(&max7219_device, true);
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
    uint8_t led_matrix_data[8];

    while(true){
        xTaskNotifyWait( 
            0x0,            // clear notify value under function entry
            0x0,            // clear notify value under function exit
            &temp_value,    // notify value
            portMAX_DELAY   // max notify wait delay
        );
        
        ESP_LOGI("", "Read ESS Temperature: %dC", temp_value);
    

    // convert temperature to led digits
    for (int i = 0; i < 4; i++) {
        led_matrix_data[i] = led_matrix_digit[temp_value % 10][i];
        led_matrix_data[i+4] = led_matrix_digit[temp_value / 10][i];
    }

    // display temperature
    max7219_draw_image_8x8(
        &max7219_device, // pointer to max7219_t structure
        0,               // start line of 8x8 
        led_matrix_data  // data to display
    );
    }
}

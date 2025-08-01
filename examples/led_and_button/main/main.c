#include "led_and_button.h"
#include "esp_log.h"


void app_main(void)
{
    
#ifdef CONFIG_GPIO_DRIVER_API
    ESP_LOGI("GPIO Driver", "Led and button example");
    configure_driver();
    while (1) {
        poll_driver();
    }
#endif

#ifdef CONFIG_GPIO_HAL_API
    ESP_LOGI("GPIO HAL", "Led and button example");
    configure_hal();
    while (1) {
        poll_hal();
    }
#endif

}

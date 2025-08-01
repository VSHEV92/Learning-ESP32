#ifndef LED_AND_BUTTON_H
#define LED_AND_BUTTON_H

#include "driver/gpio.h"
#include "hal/gpio_hal.h"
#include "sdkconfig.h"

/*
 *  Configure gpio pins. Poll button and blink led using driver level API
 */
void configure_driver();
void poll_driver();


/*
 *  Configure gpio pins. Poll button and blink led using HAL API
 */
void configure_hal();
void poll_hal();

#endif

#ifndef LED_AND_BUTTON_H
#define LED_AND_BUTTON_H

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


/*
 *  Configure gpio pins. Poll button and blink led using Low Level API
 */
void configure_ll();
void poll_ll();


/*
 *  Configure gpio pins. Poll button and blink led using SoC Macro API
 */
void configure_soc();
void poll_soc();

#endif

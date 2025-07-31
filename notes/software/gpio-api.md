# GPIO API

------

GPIO have three levels in API hierarchy:

- Driver Level
- HAL Level
- LL Level

------

## Useful Header

- **components/hal/include/hal/gpio_types.h** - define all common enums such as *gpio_mode_t*,  *gpio_pullup_t*, *gpio_pulldown_t*, *gpio_int_type_t* and so on.
- **components/soc/include/soc/gpio_periph.h** - wrapper for other soc specific headers
- **components/soc/"esp chip"/include/soc/gpio_num.h** - define gpio_num_t enum. Holds gpio numbers names.
- **components/soc/"esp chip"/include/soc/gpio_reg.h** - define low level gpio fields set/get macros and masks for gpio fields.
- **components/soc/"esp chip"/include/soc/gpio_sig_map.h** - defines that specify map output peripheral signals to GPIO Matrix Mux port
- **components/soc/"esp chip"/include/soc/gpio_struct.h** - defines gpio_dev_t struct - contain all gpio registers as C structure.
- **components/soc/"esp chip"/include/soc/gpio_sd_reg.h** - define low level gpio sigma-delta modulation fields set/get macros and masks for gpio fields.
- **components/soc/"esp chip"/include/soc/gpio_struct.h** - defines  gpio_sd_dev_t struct - contain all sigma-delta modulation gpio registers as C structure.

------

## Driver Level

Driver Level  sources locate in **components/driver/gpio**.

### Include Files

#### components/driver/gpio/include/driver/gpio.h

Driver main include file is **components/driver/gpio/include/driver/gpio.h**. 

It declare gpio configuration structure: **gpio_config_t**

- **pin_bit_mask (uint64_t)** - set with bit mask, each bit maps to a GPIO
- **mode (gpio_mode_t)**  - set input/output mode (*GPIO_MODE_DISABLE, GPIO_MODE_INPUT, GPIO_MODE_OUTPUT, GPIO_MODE_OUTPUT_OD, GPIO_MODE_INPUT_OUTPUT_OD, GPIO_MODE_INPUT_OUTPUT*)
- **pull_up_en (gpio_pullup_t)** - pull-up (*GPIO_PULLUP_DISABLE, GPIO_PULLUP_ENABLE*)
- **pull_down_en (gpio_pulldown_t)** - pull-down (*GPIO_PULLDOWN_DISABLE, GPIO_PULLDOWN_ENABLE*)
- **intr_type (gpio_int_type_t)** - interrupt type (*GPIO_INTR_DISABLE, GPIO_INTR_POSEDGE, GPIO_INTR_NEGEDGE, GPIO_INTR_ANYEDGE, GPIO_INTR_ANYEDGE, GPIO_INTR_LOW_LEVEL, GPIO_INTR_HIGH_LEVEL, GPIO_INTR_MAX*)

If also declare some usefull function:

- **gpio_config** - GPIO common configuration using gpio_config_t struct.
- **gpio_reset_pin** - reset a GPIO to a certain state (select gpio function, enable pullup and disable input and output).
- **gpio_set_level**, **gpio_get_level** - write and read gpio data
- **gpio_iomux_in**, **gpio_iomux_out** - configure IO MUX



### Source files

#### components/driver/gpio/gpio.c 

- Define all Driver Level functions. Almost all function are wrapper around HAL functions through gpio spinlock.
- Define gpio_context_t struct, that contain gpio_hal_context_t, gpio_spinlock, isr function and handler.
- Create static gpio_hal_context_t variable _gpio_hal using GPIO_HAL_GET_HW macro.
- Create static gpio_context_t variable gpio_context. Init it with _gpio_hal. 



------

## HAL Level

HAL Level  sources locate in **components/hal**.

### Include Files

#### components/hal/include/hal/gpio_type.h

Driver and HAL include file that define all common enums such as *gpio_mode_t*,  *gpio_pullup_t*, *gpio_pulldown_t*, *gpio_int_type_t* and so on.

#### components/hal/include/hal/gpio_hal.h

HAL main include file is **components/hal/include/hal/gpio_hal.h**. 

- Define GPIO_HAL_GET_HW as an alias of GPIO_LL_GET_HW macro. This macro is used to get gpio_hal_context_t static variable _gpio_hal.
- Define gpio_hal_context_t structure as wrapper around gpio_dev_t structure used in LL.
- Define almost all HAL Level functions. Almost all function are wrapper macro around LL functions.



### Source files

#### components/hal/gpio_hal.c 

- Define some HAL Level function, that are not define as macro in gpio_hal.h include file.



------

## LL Level

LL Level  sources locate in **components/hal/"esp chip"/include/hal**.

### Include Files

#### components/hal/"esp chip"/include/hal/gpio_ll.h

LL main include file is **components/hal/"esp chip"/include/hal/gpio_ll.h**. 

- Define GPIO_LL_GET_HW. Macro that return address of gpio_dev_t GPIO global variable.
- Define all LL Level functions as inline function that use soc low level macro to set and get register fields.
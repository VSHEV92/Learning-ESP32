# ESP-C3 GPIO

------

ESP-C3 GPIO system consists from three parts:

- PAD Structure
- IO MUX
- GPIO Matrix

![GPIO System](images/io-mux-gpio-matrix.png)

From software point GPIO System configured through two groups of GPIO register:

- GPIO Registers
- IO MUX Registers

------

## Data Registers

- Input data - can be read from **GPIO_IN_REG** register.
- Output data - can be write using **GPIO_OUT_REG** register. Individual bit can be set and clear using **GPIO_OUT_W1TS** and **GPIO_OUT_W1TC** registers. 

------

## PAD Structure Registers

![GPIO System](images/pad-structure.png)

PAD Structure is GPIO frontend part. It has following configurations:

- Weak Pull Up - 45 kOhm resistor. Configured in **IO_MUX_GPIOn_REG** register in **IO_MUX_GPIOn_FUN_WPU** field. 
- Weak Pull Down - 45 kOhm resistor. Configured in **IO_MUX_GPIOn_REG** register in **IO_MUX_GPIOn_FUN_WPD** field. 
- Input Enable - If disabled input is always zero because of AND gate. Configured in **IO_MUX_GPIOn_REG** register in **IO_MUX_GPIOn_FUN_IE** field. 
- Output Enable - If disabled output in third state because of tree-state buffer. Configured using **GPIO_ENABLE_REG** register. Individual bit can be set and clear using **GPIO_ENABLE_W1TS** and **GPIO_ENABLE_W1TC** registers. 
- Output Enable Inversion - if needed invert output enable signal using **GPIO_FUNCn_OEN_INV_SEL** field in **GPIO_FUNCn_OUT_SEL_CFG_REG** register.
- Output Enable Configuration - choose how output enable is configured: from peripheral or from **GPIO_ENABLE_REG** register. Choose configuration using **GPIO_FUNCn_OUT_SEL_CFG_REG** register **GPIO_FUNCn_OEN_SEL** field. 
- Driver Strength - select output current maximum value in mA. Configured in **IO_MUX_GPIOn_REG** register in **IO_MUX_GPIOn_FUN_DRV** field. 
- Open drain - to enable open drain output in the register **GPIO_PINx_REG** set **GPIO_PINx_PAD_DRIVER** field.

------

## IO MUX

IO MUX is used to directly connect some peripheral to GPIO PAD Structure. In this part input signal also filtered and synchronized.

### Input synchronization 

The signals will be synchronized to the APB bus clock by GPIO SYNC hardware, then go to GPIO matrix. This synchronization applies to all GPIO matrix signals but does not apply when using the IO MUX.

![GPIO System](images/gpio-sync.png)

Synchronization done using to registers, each can be configured for rising or falling edge clock or can be bypassed.

Synchronization configured in **GPIO_PINn_REG** register in fields **GPIO_PINn_SYNC1_BYPASS** and **GPIO_PINn_SYNC2_BYPASS**.

### Input filter

Optionally filter for pin input signals can be enabled. Only the signals with a valid width of more than two clock cycles can be sampled.

![GPIO System](images/gpio-filter-output.png)

Filter can be enabled in **IO_MUX_GPIOn_REG** register using **IO_MUX_GPIOn_FILTER_EN** field .

### Input and Output Multiplexer

Input and output signal can be connected through GPIO Matrix or Directly from peripheral. Input signal can also be routed to analog peripheral. IO MUX is used to select signal source.

IO MUX  can be configured in **IO_MUX_GPIOn_REG** register in **IO_MUX_GPIOn_MCU_SEL** field.

------

## GPIO Matrix

GPIO Matrix is used to connect every pin to every peripheral. This is gpio pin to peripheral interconnect

### Input Signals

Every gpio input can be connected to every peripheral input

- GPIO Mux - choose gpio input signal for given peripheral using **GPIO_FUNCy_IN_SEL** field in **GPIO_FUNCy_IN_SEL_CFG_REG** register.
- Input inversion - if needed invert input signal using **GPIO_FUNCn_IN_INV_SEL** field in **GPIO_FUNCy_IN_SEL_CFG_REG** register.
- Peripheral input - choose input for peripheral from GPIO Matrix or from IO MUX using **GPIO_SIGn_IN_SEL** field in **GPIO_FUNCy_IN_SEL_CFG_REG** register.

### Output Signals

Every peripheral output can be connected to every gpio output

- GPIO Mux -  choose peripheral output signal for given gpio using **GPIO_FUNCn_OUT_SEL** field in **GPIO_FUNCn_OUT_SEL_CFG_REG** register.
- Output inversion - if needed invert output signal using **GPIO_FUNCn_OUT_INV_SEL** field in **GPIO_FUNCn_OUT_SEL_CFG_REG** register.

------

## GPIO Input Programming Procedure 

1. Configure pull-up and pull down resistors using **IO_MUX_GPIOn_FUN_WPU** and **IO_MUX_GPIOn_FUN_WPD**  fields in  **IO_MUX_GPIOn_REG**. 
2. Enable input using **IO_MUX_GPIOn_FUN_IE** field in **IO_MUX_GPIOn_REG** register.  
3. Configure input filter using **IO_MUX_GPIOn_FILTER_EN** field in **IO_MUX_GPIOn_REG** register.
4. Configure synchronization using fields **GPIO_PINn_SYNC1_BYPASS** and **GPIO_PINn_SYNC2_BYPASS** in **GPIO_PINn_REG** register.
5. Read gpio value using **GPIO_IN_REG** register.

------

## GPIO Output Programming Procedure 

1. Configure output GPIO Mux to gpio output using **GPIO_FUNCn_OUT_INV_SEL** field in **GPIO_FUNCn_OUT_SEL_CFG_REG** register. Set this field to 128 (0x80).

2. If needed invert output signal using **GPIO_FUNCn_OUT_INV_SEL** field in **GPIO_FUNCn_OUT_SEL_CFG_REG** register.

3. Configure IO MUX to get data from GPIO Matrix in **IO_MUX_GPIOn_REG** register in **IO_MUX_GPIOn_MCU_SEL** field. Set this field to 1.

4. Choose output enable source using **GPIO_FUNCn_OUT_SEL_CFG_REG** register **GPIO_FUNCn_OEN_SEL** field. 

5. If needed invert output enable signal using **GPIO_FUNCn_OEN_INV_SEL** field in **GPIO_FUNCn_OUT_SEL_CFG_REG** register.

6. Configure output strength  in **IO_MUX_GPIOn_REG** register in **IO_MUX_GPIOn_FUN_DRV** field. 

7.  Choose push-pull or open-drain output in the register **GPIO_PINx_REG** set **GPIO_PINx_PAD_DRIVER** field.

8. If open-drain output and if needed configure pull-up and pull down resistors using **IO_MUX_GPIOn_FUN_WPU** and **IO_MUX_GPIOn_FUN_WPD**  fields in  **IO_MUX_GPIOn_REG**.  

9. Enable output using **GPIO_ENABLE_REG** register. Individual bit can be set and clear using **GPIO_ENABLE_W1TS** and **GPIO_ENABLE_W1TC** registers. 

10. Set gpio output value using **GPIO_OUT_REG** register. Individual bit can be set and clear using **GPIO_OUT_W1TS** and **GPIO_OUT_W1TC** registers. 

    

    

    

    

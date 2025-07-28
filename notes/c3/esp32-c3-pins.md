# ESP32-C3 Pins

[ESP-C3 Datasheet](https://www.espressif.com/sites/default/files/documentation/esp32-c3_datasheet_en.pdf)

------

### Clock Pins

**XTAL_P**, **XTAL_N** - External clock input/output connected to the chip’s crystal or oscillator. P/N means differential clock positive/negative. Main clock source.

**XTAL_32K_P (GPIO0)**, **XTAL_32K_N (GPIO1)** - 32 kHz external clock input/output connected to ESP32-C3’s crystal or oscillator. Used as the RTC slow clock is used for RTC counter, RTC watchdog and low-power controller.

------

### Reset Pins

Reset pin is **CHIP_EN**. It is active low. 

Note from datasheet: *Do not leave the CHIP_EN pin floating*. 

Need to pulled-up by external resistor and connected to push button.  

------

### Boot Pins

The chip allows for configuring the boot mode through strapping pins.

Strapping pins are latched at power-up or a hardware reset.

There are two boot modes:

- **SPI Boot mode** - the ROM bootloader loads and executes the program from SPI flash to boot the system (Normal Boot)
- **Joint Download Boot mode** - users can download binary files into flash using UART0 or USB interface. It is also possible to download binary files into SRAM and execute it from SRAM

Strapping pins:

- **GPIO2** - actually does not determine boot mode, but it is recommended to pull
  this pin up due to glitches
- **GPIO8** - supplemented pin for GPIO9. Usually connected to external pullup resistor. If GPIO9 is 1 it can be in any value. If GPIO9 is 0, it should be 1.
- **GPIO9** - most often determines boot mode. Usually pulled-up by external resistor and connected to push button.
  - 1 - SPI boot mode
  - 0 - Joint download boot mode

------

### Flash Pins

Chip has to interface to connect to SPI Flash

- **SPI0/1** - interface for connection to in-package or off-package flash via the SPI bus. For c3 with internal flash these pins should be left unconnected. Interface pins:
  - SPIQ (GPIO16) - Data out
  - SPID (GPIO17) - Data in
  - SPIHD (GPIO12) - Hold
  - SPIWP (GPIO13) - Write protect
  - SPICLK (GPIO15) - Clock
  - SPICSO (GPIO14) - Chip select
- **SPI2** - interface for fast SPI connection. Interface pins:
  - FSPIQ (GPIO2) - Data out
  - FSPID (GPIO7) - Data in
  - FSPIHD (GPIO4) - Hold
  - FSPIWP (GPIO5) - Write protect
  - FSPICLK (GPIO6) - Clock
  - FSPICS0 (GPIO10) - Chip select

------

### RF Pins

**LNA_IN** - Low Noise Amplifier (RF LNA) input / output signals. Used for antenna connection

------

### USB Pins

Chip has two pins for USB Serial/JTAG function. 

Pins: USB_D+ (GPIO19), USB_D- (GPIO18),

To function as GPIOs, these pins need to be reconfigured. 

------

### JTAG Pins

Often used for debugging. To free these pins up, the pin functions USB_D+/- of the USB Serial/JTAG Controller can be used instead.

Pins: MTMS (GPIO 4), MTCK (GPIO 6), MTDI (GPIO 5) ,MTDO (GPIO 7).

------

### UART Pins

UART0 interface - often used for debugging. Direct connect to periferal. 

Pins: U0TXD (GPIO 21), U0RXD (GPIO 20)

------

### Power Pins

- **VDD3P3** - This is the main digital power supply for the ESP32-C3. It powers most of the digital components, including the CPU, memory, and digital peripherals.
- **VDDA** - This is the power supply for the analog components of the ESP32-C3, such as the ADC (Analog-to-Digital Converter) and DAC (Digital-to-Analog Converter).
- **VDD3P3_CPU** - This is likely the power supply specifically for the CPU core. It is closely related to VDD3P3 and might be derived from it. The separation might be for power management and voltage regulation purposes, ensuring a stable voltage for the CPU core, which is critical for its operation.
- **VDD3P3_RTC** - This pin powers the RTC (Real-Time Clock) and other analog components within the RTC domain. The RTC is responsible for keeping track of time even when the main power is off. It also powers other analog components within the RTC domain.
- **VDD_SPI** - This pin is primarily used to power the external SPI flash memory connected to the ESP32-C3. It can also be used as a general-purpose 3.3V output pin for other components.

------


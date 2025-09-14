# Bootloader Override Example

------

**call_start_cpu0** - bootloader entry function

------

## bootloader_init()

1. **bootloader_hardware_init()**
   - check chip revision using **efuse_hal_chip_revision()**
2. **bootloader_ana_reset_config()** 
   - **bootloader_ana_super_wdt_reset_config()** - enable reset super watchdog timer
   -  **bootloader_ana_bod_reset_config()** - enable reset from brown-out detector
   - **bootloader_ana_clock_glitch_reset_config()** - enable reset from XTAL clock glitch detector
3. **bootloader_super_wdt_auto_feed()** - enable super watchdog timer auto feed from super watchdog timer controller 
4. If **CONFIG_APP_BUILD_TYPE_RAM** is not selected and loaded with JTAG, ESP-IDF does not contain all the startup code required to initialize the CPUs and ROM memory (data/bss):
   - **bootloader_init_mem(**) - if needed configure memory protection
   - **bootloader_clear_bss_section()** - zeroing bss code section
5. if **CONFIG_EFUSE_VIRTUAL** is enable (all efuse operation are virtual, work with ram and not burn efsue)
   - **esp_efuse_init_virtual_mode_in_ram()** - init ram with virtual efises
6. **bootloader_clock_configure()** - configure clocks
   - **esp_rom_uart_tx_wait_idle()** - wait until all rom uart messages are printed
   - **rtc_clk_init()** - configure slow and fast RTC clocks
   - disable interrupts and clear status bits all RTC sources (watchdogs, brown-out, glitch detector)
7. **bootloader_console_init()** - configure console according to **CONFIG_ESP_CONSOLE_*** configuration
   - **CONFIG_ESP_CONSOLE_NONE** - disable both rom putc functions
   - **CONFIG_ESP_CONSOLE_UART** - configure uart periferal to message printing. Install UART1 as default console. Options can be overrided, if **CONFIG_ESP_CONSOLE_UART_CUSTOM** configuration is set.
   - **CONFIG_ESP_CONSOLE_USB_CDC** - print message using usb-to-uart
   - **CONFIG_ESP_CONSOLE_USB_SERIAL_JTAG** - print message using usb-to-jtag 
8. **bootloader_print_banner()** - print bootloader banner (bootloader descriptor, compile time and so on)
9. If **CONFIG_APP_BUILD_TYPE_PURE_RAM_APP** (don't use flash at all) not set, then initialize cache and MMU:
   - **cache_hal_init()** - enable caches and autoload
   - **mmu_hal_init()** - set page size and invalidate all entries
   - **bootloader_flash_update_id()** - read flash id for debug messages in future
   - **bootloader_flash_xmc_startup()** - run special procedure if flash is XMC device
   - **bootloader_init_spi_flash()** - configure flash. Setup flash pins and timings, print flash info, enable write protection
10. **bootloader_check_wdt_reset()** - check if reset was because of watchdog timer, and if it is true then print reset info
11. **bootloader_config_wdt()** - disable flashboot bits for main watchdog 0 and RTC watchdog timers. This watchdogs are enabled while ROM boot
12. **bootloader_enable_random()** - enable random number generator (RNG)



If any procedure inside **bootloader_init()** function not return ESP_OK, then call **bootloader_reset()**, to cause chip software reset

------

## Select Boot Partition

TODO

------

## Load Image

TODO

# Application Startup

------

**call_start_cpu0** - application entry function

------

## call_start_cpu0()

1. Set global pointer register
2. Initialize .bss sections in RAM and RTC_RAM
3. Initialize cache MMU if it is not done earlier by bootloader (BUILD TYPE RAM)
4. Initialize SPI flash interface if it is not done earlier by bootloader (BUILD TYPE RAM)
5. Disable watchdog timer if it was the reason of last reset
6. **esp_rtc_init()** - configure power related stuff (RTC_CNTL registers setup)
7. Configure MSPI flash and MMU map according to application needs
8. Configure memory regions protection if needed
9. **esp_clk_init()** - setup CPU and RTC fast and slow clocks (source, frequency)
10. **esp_perip_clk_init()** - enable clocks for all peripheral, RF and internal modules
11. **core_intr_matrix_clear()** - set all interrupt sources to INVALID values
12. SYS_STARTUP_FN() - run this macro to call **start_cpu0_default()** function

------

## start_cpu0_default() 

1. Print CPU clock frequency
2. Print application image info - name, version, compile time, IDF version, chip revision and so on
3. **do_core_init()** - Initialize core components and services
   - 

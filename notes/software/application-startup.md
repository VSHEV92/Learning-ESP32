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
   - **heap_caps_init()** - Initialize heap allocator
   - **esp_timer_early_init()** - configure timer from TIMG
   - **esp_newlib_init()** - Includes defining the syscall table, setting up any common locks, etc.
   - **esp_newlib_time_init()** - Initialize newlib time functions ( same as esp_timer_early_init() )
   - **esp_flash_app_init()** - Enable OS-level SPI flash protections in IDF
4.  **do_global_ctors()** - Execute global C++ constructors
5. **do_secondary_init()** - Execute init functions of other components
   - For multicore devices startup other cores
   - **do_system_init_fn()** - Execute initialization functions esp_system_init_fn_t assigned to the main core. Call component init functions defined using **ESP_SYSTEM_INIT_Fn** macros
6. Disable boot watchdog timer started by bootloader
7. Start application by calling **esp_startup_start_app()** function

------

##  esp_startup_start_app()

1. In needed enable interrupt watchdog timer
2. If needed configure interrupt matrix to enable cross-core interrupts
3. Create main task, that run **main_task()** function
4. If needed run **port_start_app_hook()** callback
5. Start FreeRTOS scheduler, that jump to **main_task()**

------

## main_task()

1. If needed wait for FreeRTOS initialization to finish on other core
2. If needed configure task watchdog timer
3. Run **app_main()** function

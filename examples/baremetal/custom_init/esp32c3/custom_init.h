#ifndef CUSTOM_INIT_H
#define CUSTOM_INIT_H

#include <hal/rwdt_ll.h>
#include <hal/mwdt_ll.h>

#include <soc/clk_tree_defs.h>
#include <soc/system_reg.h>

// ----------------------------------------
//    Disable boot RTC watchdog timer
// ----------------------------------------
#define custom_init_rwdt_disable() {                                                    \
    /* 1. Load to write protect register anlock key 0x50D83AA1 */                       \
    REG_WRITE(RTC_CNTL_WDTWPROTECT_REG, RTC_CNTL_WDT_WKEY_VALUE);                       \
                                                                                        \
    /* 2. Disable RTC boot watchdog timer */                                            \
    REG_SET_FIELD(RTC_CNTL_WDTCONFIG0_REG, RTC_CNTL_WDT_FLASHBOOT_MOD_EN, 0);           \
                                                                                        \
    /* 3. Lock write protection by loading value other then key 0x50D83AA1 */           \
    REG_WRITE(RTC_CNTL_WDTWPROTECT_REG, ~RTC_CNTL_WDT_WKEY_VALUE);                      \
}
    
// ----------------------------------------
//    Disable boot main watchdog timer 0
// ----------------------------------------
#define custom_init_mwdt0_disable() {                                                   \
    /* 1. Load to write protect register anlock key 0x50D83AA1 */                       \
    REG_WRITE(TIMG_WDTWPROTECT_REG(0), TIMG_WDT_WKEY_VALUE);                            \
                                                                                        \
    /* 2. Disable main 0 boot watchdog timer */                                         \
    REG_SET_FIELD(TIMG_WDTCONFIG0_REG(0), TIMG_WDT_FLASHBOOT_MOD_EN, 0);                \
                                                                                        \
    /* 3. Lock write protection by loading value other then key 0x50D83AA1 */           \
    REG_WRITE(TIMG_WDTWPROTECT_REG(0), ~TIMG_WDT_WKEY_VALUE);                           \
}


// ----------------------------------------
//    Configure SOC clock frequency
// ----------------------------------------

// Clock configuration
// 1. CPU clock source - PLL
// 2. PLL clock frequency - 320 MHz
// 3. CPU clock frequency - 16- MHZ

// Derived clocks
// 1. APB Clock is set to 80 MHz

#define custom_init_cpu_clock_configure() {                                             \
    /* 1. Set CPU clock source to PLL */                                                \
    REG_SET_FIELD(SYSTEM_SYSCLK_CONF_REG, SYSTEM_SOC_CLK_SEL, SOC_CPU_CLK_SRC_PLL);     \
    /* 2. Set PLL frequency to 320 MHz */                                               \
    REG_SET_FIELD(SYSTEM_CPU_PER_CONF_REG, SYSTEM_PLL_FREQ_SEL, 0);                     \
    /* 3. Set CPU frequency to 160 MHz */                                               \
    REG_SET_FIELD(SYSTEM_CPU_PER_CONF_REG, SYSTEM_CPUPERIOD_SEL, 1);                    \
}

// ----------------------------------------
//          Main custom init macro
// ----------------------------------------
#define custom_init() {                                                                 \
    custom_init_rwdt_disable();                                                         \
    custom_init_mwdt0_disable();                                                        \
    custom_init_cpu_clock_configure();                                                  \
}

#endif

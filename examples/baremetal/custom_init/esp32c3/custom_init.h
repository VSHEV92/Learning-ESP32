#ifndef CUSTOM_INIT_H
#define CUSTOM_INIT_H

#include <hal/rwdt_ll.h>
#include <hal/mwdt_ll.h>

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
    /* 2 Disable main 0 boot watchdog timer */                                          \
    REG_SET_FIELD(TIMG_WDTCONFIG0_REG(0), TIMG_WDT_FLASHBOOT_MOD_EN, 0);                \
                                                                                        \
    /* 3. Lock write protection by loading value other then key 0x50D83AA1 */           \
    REG_WRITE(TIMG_WDTWPROTECT_REG(0), ~TIMG_WDT_WKEY_VALUE);                           \
}


// ----------------------------------------
//          Main custom init macro
// ----------------------------------------
#define custom_init() {                                                                 \
    custom_init_rwdt_disable();                                                         \
    custom_init_mwdt0_disable();                                                        \
}

#endif

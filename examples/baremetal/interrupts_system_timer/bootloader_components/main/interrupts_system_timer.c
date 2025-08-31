#include "esp_log.h"
#include "stdbool.h"
#include "sdkconfig.h"
#include "custom_init.h"
#include "riscv/rv_utils.h"
#include "riscv/interrupt.h"
#include "hal/systimer_ll.h"

extern uint32_t _custom_vector_table;

#define SYSTIMER_UNIT0 0
#define SYSTIMER_TARGET0 0

#define INTERRUPT_ID 1

static const char *TAG = "ISR";

static systimer_dev_t* systimer_dev = &SYSTIMER;

void custom_systimer_isr(void* arg);
void configure_interrupts();
void configure_system_timer();

void __attribute__((noreturn)) call_start_cpu0(void) {
    // run custom soc init
    custom_init();
    
    // setup interrupts
    configure_interrupts();

    // setup and run system timer
    configure_system_timer();

    while(1){}
}


void custom_systimer_isr(void* arg) {
    systimer_dev_t* systimer_device = (systimer_dev_t*)arg;
    systimer_ll_clear_alarm_int(systimer_device, SYSTIMER_TARGET0);
    ESP_DRAM_LOGI(TAG, "System timer interrupt");
}


void configure_interrupts() {
    // --------------------------------------------
    // Register handler for corresponding interrupt
    // --------------------------------------------
    intr_handler_set(INTERRUPT_ID, custom_systimer_isr, (void*)systimer_dev);

    // --------------------------------------------
    //            Configure interrupts
    // --------------------------------------------

    // 1. Disable interrupts globally (MSTATUS.MIE)
    RV_CLEAR_CSR(mstatus, MSTATUS_MIE);

    // 2. Set vector table address (MTVEC)
    rv_utils_set_mtvec((uint32_t)(&_custom_vector_table));

    // 3. Set interrupt type to level interrupt
    esprv_intc_int_set_type(INTERRUPT_ID, INTR_TYPE_LEVEL);

    // 4. Set interrupt priority to lowest value 1
    esprv_intc_int_set_priority(INTERRUPT_ID, 1);

    // 5. Configure interrupt peripheral source 
    intr_matrix_route(ETS_SYSTIMER_TARGET0_INTR_SOURCE, INTERRUPT_ID);

    // 6. Enable interrupt in controller
    esprv_intc_int_enable(1 << INTERRUPT_ID);

    // 7. Set interrupt threshold to 1, so that interrupt can be fired 
    esprv_intc_int_set_threshold(0);

    // 8. Run FENCE instruction so that all memory operations with interrupt controller registers are finished
    rv_utils_memory_barrier();

    // 9. Enable interrupts globally (MSTATUS.MIE)
    RV_SET_CSR(mstatus, MSTATUS_MIE);
}


void configure_system_timer() {
    // in esp32c3 system timer period is always 16 MHz
    uint64_t period_ticks = 16000000 / 1000 * CONFIG_SYSTEM_TIMER_PERIOD_MS; 

    // 1. Enable system timer clock
    systimer_ll_enable_clock(systimer_dev, true);
    
    // 2. Disable counter
    systimer_ll_enable_counter(systimer_dev, SYSTIMER_UNIT0, false);

    // 3. Set counter initial value
    systimer_ll_set_counter_value(systimer_dev, SYSTIMER_UNIT0, 0);
    systimer_ll_apply_counter_value(systimer_dev, SYSTIMER_UNIT0);

    // 4. Configure comparator
    systimer_ll_connect_alarm_counter(systimer_dev, SYSTIMER_TARGET0, SYSTIMER_UNIT0);
    systimer_ll_enable_alarm_period(systimer_dev, SYSTIMER_TARGET0);
    systimer_ll_set_alarm_period(systimer_dev, SYSTIMER_TARGET0, period_ticks);
    systimer_ll_apply_alarm_value(systimer_dev, SYSTIMER_TARGET0);

    // 5. Enable comparator
    systimer_ll_enable_alarm(systimer_dev, SYSTIMER_TARGET0, true);

    // 6. Enable comparator interrupt
    systimer_ll_enable_alarm_int(systimer_dev, SYSTIMER_TARGET0, true);
    
    // 7. Enable counter
    systimer_ll_enable_counter(systimer_dev, SYSTIMER_UNIT0, true);
}

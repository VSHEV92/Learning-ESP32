#include "FreeRTOS.h"
#include "task.h"
#include "portmacro.h"
#include "string.h"

#include "hal/systimer_ll.h"
#include "riscv/rv_utils.h"
#include "riscv/interrupt.h"
#include "soc/interrupts.h"

/* ESP32 port */
#define RV_EXTERNAL_INT_OFFSET  0
#define SYSTIMER_UNIT0 0
#define SYSTIMER_TARGET0 0
#define INTERRUPT_ID 1

extern uint32_t _custom_vector_table;
static systimer_dev_t* systimer_dev = &SYSTIMER;

/* Let the user override the pre-loading of the initial RA. */
#ifdef configTASK_RETURN_ADDRESS
    #define portTASK_RETURN_ADDRESS    configTASK_RETURN_ADDRESS
#else
    #define portTASK_RETURN_ADDRESS    0
#endif

/* The stack used by interrupt service routines.  Set configISR_STACK_SIZE_WORDS
 * to use a statically allocated array as the interrupt stack.  Alternative leave
 * configISR_STACK_SIZE_WORDS undefined and update the linker script so that a
 * linker variable names __freertos_irq_stack_top has the same value as the top
 * of the stack used by main.  Using the linker script method will repurpose the
 * stack that was used by main before the scheduler was started for use as the
 * interrupt stack after the scheduler has started. */
#ifdef configISR_STACK_SIZE_WORDS
static __attribute__( ( aligned( 16 ) ) ) StackType_t xISRStack[ configISR_STACK_SIZE_WORDS ] = { 0 };
const StackType_t xISRStackTop = ( StackType_t ) &( xISRStack[ configISR_STACK_SIZE_WORDS & ~portBYTE_ALIGNMENT_MASK ] );

/* Don't use 0xa5 as the stack fill bytes as that is used by the kernel for
 * the task stacks, and so will legitimately appear in many positions within
 * the ISR stack. */
    #define portISR_STACK_FILL_BYTE    0xee
#else
    extern const uint32_t __freertos_irq_stack_top[];
    const StackType_t xISRStackTop = ( StackType_t ) __freertos_irq_stack_top;
#endif

/*
 * Setup the timer to generate the tick interrupts.  The implementation in this
 * file is weak to allow application writers to change the timer used to
 * generate the tick interrupt.
 */
void vPortSetupTimerInterrupt( void ) __attribute__( ( weak ) );

/*-----------------------------------------------------------*/
const size_t uxTimerIncrementsForOneTick = ( size_t ) ( ( configCPU_CLOCK_HZ ) / ( configTICK_RATE_HZ ) ); /* Assumes increment won't go over 32-bits. */

/* Holds the critical nesting value - deliberately non-zero at start up to
 * ensure interrupts are not accidentally enabled before the scheduler starts. */
size_t xCriticalNesting = ( size_t ) 0xaaaaaaaa;
size_t * pxCriticalNesting = &xCriticalNesting;

/* Used to catch tasks that attempt to return from their implementing function. */
size_t xTaskReturnAddress = ( size_t ) portTASK_RETURN_ADDRESS;

/* Set configCHECK_FOR_STACK_OVERFLOW to 3 to add ISR stack checking to task
 * stack checking.  A problem in the ISR stack will trigger an assert, not call
 * the stack overflow hook function (because the stack overflow hook is specific
 * to a task stack, not the ISR stack). */
#if defined( configISR_STACK_SIZE_WORDS ) && ( configCHECK_FOR_STACK_OVERFLOW > 2 )
    #warning "This path not tested, or even compiled yet."

    static const uint8_t ucExpectedStackBytes[] =
    {
        portISR_STACK_FILL_BYTE, portISR_STACK_FILL_BYTE, portISR_STACK_FILL_BYTE, portISR_STACK_FILL_BYTE, \
        portISR_STACK_FILL_BYTE, portISR_STACK_FILL_BYTE, portISR_STACK_FILL_BYTE, portISR_STACK_FILL_BYTE, \
        portISR_STACK_FILL_BYTE, portISR_STACK_FILL_BYTE, portISR_STACK_FILL_BYTE, portISR_STACK_FILL_BYTE, \
        portISR_STACK_FILL_BYTE, portISR_STACK_FILL_BYTE, portISR_STACK_FILL_BYTE, portISR_STACK_FILL_BYTE, \
        portISR_STACK_FILL_BYTE, portISR_STACK_FILL_BYTE, portISR_STACK_FILL_BYTE, portISR_STACK_FILL_BYTE
    }; \

    #define portCHECK_ISR_STACK()    configASSERT( ( memcmp( ( void * ) xISRStack, ( void * ) ucExpectedStackBytes, sizeof( ucExpectedStackBytes ) ) == 0 ) )
#else /* if defined( configISR_STACK_SIZE_WORDS ) && ( configCHECK_FOR_STACK_OVERFLOW > 2 ) */
    /* Define the function away. */
    #define portCHECK_ISR_STACK()
#endif /* configCHECK_FOR_STACK_OVERFLOW > 2 */


void clear_timer_interrupt() {
    systimer_ll_clear_alarm_int(systimer_dev, SYSTIMER_TARGET0);
}

void intr_matrix_route(int intr_src, int intr_num) {
    if (rv_utils_get_core_id() == 0) {
        REG_WRITE(DR_REG_INTERRUPT_CORE0_BASE + 4 * intr_src, intr_num + RV_EXTERNAL_INT_OFFSET);
    }
}

void vPortSetupTimerInterrupt( void )
{
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

    // 7. Set interrupt threshold to 0, so that interrupt can be fired 
    esprv_intc_int_set_threshold(0);

    // 8. Run FENCE instruction so that all memory operations with interrupt controller registers are finished
    rv_utils_memory_barrier();


    // --------------------------------------------
    //           Configure system timer
    // --------------------------------------------

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
    systimer_ll_set_alarm_period(systimer_dev, SYSTIMER_TARGET0, uxTimerIncrementsForOneTick);
    systimer_ll_apply_alarm_value(systimer_dev, SYSTIMER_TARGET0);

    // 5. Enable comparator
    systimer_ll_enable_alarm(systimer_dev, SYSTIMER_TARGET0, true);

    // 6. Enable comparator interrupt
    systimer_ll_enable_alarm_int(systimer_dev, SYSTIMER_TARGET0, true);
    
    // 7. Enable counter
    systimer_ll_enable_counter(systimer_dev, SYSTIMER_UNIT0, true);
}


BaseType_t xPortStartScheduler( void )
{
    extern void xPortStartFirstTask( void );

    #if ( configASSERT_DEFINED == 1 )
    {
        /* Check alignment of the interrupt stack - which is the same as the
         * stack that was being used by main() prior to the scheduler being
         * started. */
        configASSERT( ( xISRStackTop & portBYTE_ALIGNMENT_MASK ) == 0 );

        #ifdef configISR_STACK_SIZE_WORDS
        {
            memset( ( void * ) xISRStack, portISR_STACK_FILL_BYTE, sizeof( xISRStack ) );
        }
        #endif /* configISR_STACK_SIZE_WORDS */
    }
    #endif /* configASSERT_DEFINED */

    vPortSetupTimerInterrupt();

    xPortStartFirstTask();

    /* Should not get here as after calling xPortStartFirstTask() only tasks
     * should be executing. */
    return pdFAIL;
}
/*-----------------------------------------------------------*/

void vPortEndScheduler( void )
{
    /* Not implemented. */
    for( ; ; )
    {
    }
}
/*-----------------------------------------------------------*/

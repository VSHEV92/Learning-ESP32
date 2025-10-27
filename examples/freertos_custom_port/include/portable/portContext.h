#ifndef PORTCONTEXT_H
#define PORTCONTEXT_H

#if __riscv_xlen == 64
    #define portWORD_SIZE    8
    #define store_x          sd
    #define load_x           ld
#elif __riscv_xlen == 32
    #define store_x          sw
    #define load_x           lw
    #define portWORD_SIZE    4
#else
    #error Assembler did not define __riscv_xlen
#endif


#define portasmADDITIONAL_CONTEXT_SIZE    0
#define portCONTEXT_SIZE               ( 31 * portWORD_SIZE )
#define portCRITICAL_NESTING_OFFSET    30

/*-----------------------------------------------------------*/

.extern pxCurrentTCB
.extern xISRStackTop
.extern xCriticalNesting
.extern pxCriticalNesting

/*-----------------------------------------------------------*/

   .macro portcontextSAVE_CONTEXT_INTERNAL
addi sp, sp, -portCONTEXT_SIZE
store_x x1,  2  * portWORD_SIZE( sp )
store_x x5,  3  * portWORD_SIZE( sp )
store_x x6,  4  * portWORD_SIZE( sp )
store_x x7,  5  * portWORD_SIZE( sp )
store_x x8,  6  * portWORD_SIZE( sp )
store_x x9,  7  * portWORD_SIZE( sp )
store_x x10, 8  * portWORD_SIZE( sp )
store_x x11, 9  * portWORD_SIZE( sp )
store_x x12, 10 * portWORD_SIZE( sp )
store_x x13, 11 * portWORD_SIZE( sp )
store_x x14, 12 * portWORD_SIZE( sp )
store_x x15, 13 * portWORD_SIZE( sp )
store_x x16, 14 * portWORD_SIZE( sp )
store_x x17, 15 * portWORD_SIZE( sp )
store_x x18, 16 * portWORD_SIZE( sp )
store_x x19, 17 * portWORD_SIZE( sp )
store_x x20, 18 * portWORD_SIZE( sp )
store_x x21, 19 * portWORD_SIZE( sp )
store_x x22, 20 * portWORD_SIZE( sp )
store_x x23, 21 * portWORD_SIZE( sp )
store_x x24, 22 * portWORD_SIZE( sp )
store_x x25, 23 * portWORD_SIZE( sp )
store_x x26, 24 * portWORD_SIZE( sp )
store_x x27, 25 * portWORD_SIZE( sp )
store_x x28, 26 * portWORD_SIZE( sp )
store_x x29, 27 * portWORD_SIZE( sp )
store_x x30, 28 * portWORD_SIZE( sp )
store_x x31, 29 * portWORD_SIZE( sp )

load_x t0, xCriticalNesting                                   /* Load the value of xCriticalNesting into t0. */
store_x t0, portCRITICAL_NESTING_OFFSET * portWORD_SIZE( sp ) /* Store the critical nesting value to the stack. */

csrr t0, mstatus
store_x t0, 1 * portWORD_SIZE( sp )

load_x t0, pxCurrentTCB          /* Load pxCurrentTCB. */
store_x sp, 0 ( t0 )             /* Write sp to first TCB member. */

   .endm
/*-----------------------------------------------------------*/

   .macro portcontextSAVE_EXCEPTION_CONTEXT
portcontextSAVE_CONTEXT_INTERNAL
csrr a0, mcause
csrr a1, mepc
addi a1, a1, 4          /* Synchronous so update exception return address to the instruction after the instruction that generated the exception. */
store_x a1, 0 ( sp )    /* Save updated exception return address. */
load_x sp, xISRStackTop /* Switch to ISR stack. */
   .endm
/*-----------------------------------------------------------*/

   .macro portcontextSAVE_INTERRUPT_CONTEXT
portcontextSAVE_CONTEXT_INTERNAL
csrr a0, mcause
csrr a1, mepc
store_x a1, 0 ( sp )    /* Asynchronous interrupt so save unmodified exception return address. */
load_x sp, xISRStackTop /* Switch to ISR stack. */
   .endm
/*-----------------------------------------------------------*/

   .macro portcontextRESTORE_CONTEXT
load_x t1, pxCurrentTCB /* Load pxCurrentTCB. */
load_x sp, 0 ( t1 )     /* Read sp from first TCB member. */

/* Load mepc with the address of the instruction in the task to run next. */
load_x t0, 0 ( sp )
csrw mepc, t0

/* Restore mstatus register. */
load_x t0, 1 * portWORD_SIZE( sp )
csrw mstatus, t0

load_x t0, portCRITICAL_NESTING_OFFSET * portWORD_SIZE( sp ) /* Obtain xCriticalNesting value for this task from task's stack. */
load_x t1, pxCriticalNesting                                 /* Load the address of xCriticalNesting into t1. */
store_x t0, 0 ( t1 )                                         /* Restore the critical nesting value for this task. */

load_x x1,  2  * portWORD_SIZE( sp )
load_x x5,  3  * portWORD_SIZE( sp )
load_x x6,  4  * portWORD_SIZE( sp )
load_x x7,  5  * portWORD_SIZE( sp )
load_x x8,  6  * portWORD_SIZE( sp )
load_x x9,  7  * portWORD_SIZE( sp )
load_x x10, 8  * portWORD_SIZE( sp )
load_x x11, 9  * portWORD_SIZE( sp )
load_x x12, 10 * portWORD_SIZE( sp )
load_x x13, 11 * portWORD_SIZE( sp )
load_x x14, 12 * portWORD_SIZE( sp )
load_x x15, 13 * portWORD_SIZE( sp )
load_x x16, 14 * portWORD_SIZE( sp )
load_x x17, 15 * portWORD_SIZE( sp )
load_x x18, 16 * portWORD_SIZE( sp )
load_x x19, 17 * portWORD_SIZE( sp )
load_x x20, 18 * portWORD_SIZE( sp )
load_x x21, 19 * portWORD_SIZE( sp )
load_x x22, 20 * portWORD_SIZE( sp )
load_x x23, 21 * portWORD_SIZE( sp )
load_x x24, 22 * portWORD_SIZE( sp )
load_x x25, 23 * portWORD_SIZE( sp )
load_x x26, 24 * portWORD_SIZE( sp )
load_x x27, 25 * portWORD_SIZE( sp )
load_x x28, 26 * portWORD_SIZE( sp )
load_x x29, 27 * portWORD_SIZE( sp )
load_x x30, 28 * portWORD_SIZE( sp )
load_x x31, 29 * portWORD_SIZE( sp )
addi sp, sp, portCONTEXT_SIZE

mret
   .endm
/*-----------------------------------------------------------*/

#endif /* PORTCONTEXT_H */

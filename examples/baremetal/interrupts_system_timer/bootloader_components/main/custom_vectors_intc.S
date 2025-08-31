#include "sdkconfig.h"
#include "soc/soc.h"


    /* Handlers defined in the `vector.S` file, common to all RISC-V targets */
    .global _interrupt_handler
    .global _panic_handler

    .section .iram.text

    /* This is the vector table. MTVEC points here.
     *
     * Use 4-byte instructions here. 1 instruction = 1 entry of the table.
     * The CPU jumps to MTVEC (i.e. the first entry) in case of an exception,
     * and (MTVEC & 0xfffffffc) + (mcause & 0x7fffffff) * 4, in case of an interrupt.
     *
     * Note: for our CPU, we need to place this on a 256-byte boundary, as CPU
     * only uses the 24 MSBs of the MTVEC, i.e. (MTVEC & 0xffffff00).
     */
    .balign 0x100

    /* Since each entry must take 4-byte, let's temporarily disable the compressed
     * instruction set that could potentially generate 2-byte instructions. */
    .option push
    .option norvc

    .global _custom_vector_table
    .type _custom_vector_table, @function
_custom_vector_table:
    j _panic_handler            /* 0: Exception entry */
    j _interrupt_handler        /* 1: Free interrupt number */
    j _interrupt_handler        /* 2: Free interrupt number */
    j _interrupt_handler        /* 3: Free interrupt number */
    j _interrupt_handler        /* 4: Free interrupt number */
    j _interrupt_handler        /* 5: Free interrupt number */
    j _interrupt_handler        /* 6: Free interrupt number */
    j _interrupt_handler        /* 7: Free interrupt number */
    j _interrupt_handler        /* 8: Free interrupt number */
    j _interrupt_handler        /* 9: Free interrupt number */
    j _interrupt_handler        /* 10: Free interrupt number */
    j _interrupt_handler        /* 11: Free interrupt number */
    j _interrupt_handler        /* 12: Free interrupt number */
    j _interrupt_handler        /* 13: Free interrupt number */
    j _interrupt_handler        /* 14: Free interrupt number */
    j _interrupt_handler        /* 15: Free interrupt number */
    j _interrupt_handler        /* 16: Free interrupt number */
    j _interrupt_handler        /* 17: Free interrupt number */
    j _interrupt_handler        /* 18: Free interrupt number */
    j _interrupt_handler        /* 19: Free interrupt number */
    j _interrupt_handler        /* 20: Free interrupt number */
    j _interrupt_handler        /* 21: Free interrupt number */
    j _interrupt_handler        /* 22: Free interrupt number */
    j _interrupt_handler        /* 23: Free interrupt number */
    j _interrupt_handler        /* 24: Free interrupt number */
    j _interrupt_handler        /* 25: Free interrupt number */
    j _interrupt_handler        /* 26: Free interrupt number */
    j _interrupt_handler        /* 27: Free interrupt number */
    j _interrupt_handler        /* 28: Free interrupt number */
    j _interrupt_handler        /* 29: Free interrupt number */
    j _interrupt_handler        /* 30: Free interrupt number */
    j _interrupt_handler        /* 31: Free interrupt number */

    .size _custom_vector_table, .-_custom_vector_table

    /* Re-enable the compressed instruction set it is was enabled before */
    .option pop

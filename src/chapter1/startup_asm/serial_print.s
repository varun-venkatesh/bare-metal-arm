/* 
 * Directive .syntax unified is good practice - not needed.
 * Cortex-M3 operates in thumb mode 
 * but GNU Assembler requires .thumb directive at the start
 * and .thumb_func directive preceding each function call
 * Refer: https://stackoverflow.com/questions/11526062/mixing-arm-and-thumb-instructions
 */
 
.syntax unified
.thumb

.global _serial_print
/* Address of UART0 on Stellaris LMS36965
 * Refer: Refer: http://www.ti.com/lit/ds/symlink/lm3s6965.pdf Table 2-4
 */
.set LM3S_UART0, 0x4000C000 

.text
.align
.thumb_func
/* 
 * This function prints the string passed to it 
 * via register r0 from the caller.
 */
_serial_print:
    ldr r1, =LM3S_UART0

print_loop:
    ldrb r3, [r0], #1
    strb r3, [r1]
    cmp r3, #0
    bne _serial_print

    bx lr

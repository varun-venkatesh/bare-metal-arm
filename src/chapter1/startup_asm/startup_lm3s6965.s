/*
*  Directive .syntax unified is good practice - not needed.
*  Cortex-M3 operates in thumb mode 
*  but GNU Assembler requires .thumb directive at the start
*  and .thumb_func directive preceding each function call
*  Refer: https://stackoverflow.com/questions/11526062/mixing-arm-and-thumb-instructions
*/
.syntax unified
.thumb

/*
 * The vector table is an array of word data inside the system memory, 
 * each representing the starting address of one exception type.
 * Vector table for arm Cortex-M3
 * When an exception takes place, the processor etches the exception vector 
 * (the starting address of the exception handler) from the vector table.
 * First vector value is placed in r13 (sp). 
 * Point it to the end of SRAM on lm3s6965evb. Refer: http://www.ti.com/lit/ds/symlink/lm3s6965.pdf Table 2-4
 * Note that the vector values must be 4 bytes - hence using the directive .word
 */

.section .vectors, "x"
.word _sram_stacktop     /* 00: Reset value of the Main Stack Pointer */
.word _startup           /* 01: Reset value of the Program Counter (Reset) */
.word _hang              /* 02: Non-Maskable Interrupt */
.word _hang              /* 03: Hard Fault Interrupt */
.word _hang              /* 04: Memory Mgmt Interrupt */
.word _hang              /* 05: Bus Fault Interrupt */
.word _hang              /* 06: Usage Fault Interrupt */
.word _hang              /* 07: Reserved */
.word _hang              /* 08: Reserved */
.word _hang              /* 09: Reserved */
.word _hang              /* 10: Reserved */
.word _hang              /* 11: SVCall Interrupt */
.word _hang              /* 12: Debug Monitor Interrupt */
.word _hang              /* 13: Reserved */
.word _hang              /* 14: PendSV Interrupt */
.word _hang              /* 15: SysTick Interrupt */

.section .rodata, "aS"
str: .asciz "Hello, World\n"

/* text section begins */

.text
.align
.thumb_func
_startup:
/* zero all general-purpose registers */
    mov r0, #0
    mov r1, #0
    mov r2, #0
    mov r3, #0
    mov r4, #0
    mov r5, #0
    mov r6, #0
    mov r7, #0
    mov r8, #0
    mov r9, #0
    mov r10, #0
    mov r11, #0
    mov r12, #0

data_relo:
    ldr r0, =_flash_sdata
    ldr r1, =_sram_sdata
    ldr r2, =_sram_edata

/* handle case where data section size is zero */
    cmp r2, r1
    beq bss_relo

data_loop:
    ldrb r4, [r0], #1
    strb r4, [r1], #1
    cmp r2, r1
    bne data_loop
       
bss_relo:
    mov r0, #0 
    ldr r1, =_sram_sbss
    ldr r2, =_sram_ebss

/* handle case where bss section size is zero */
    cmp r2, r1
    beq message 

bss_loop:
    strb r0, [r1], #1
    cmp r2, r1
    bne bss_loop

message:
    ldr r0, =str
    bl _serial_print
        
_hang:
    b .


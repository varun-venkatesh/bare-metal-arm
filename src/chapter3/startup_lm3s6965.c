#include <stdint.h>
#include "irq.h"

/* Linker symbols for TI Stellaris LM3S6965 
 * defining start/end of various sections
 * using long as ANSI C guarentees long to be at least 4 bytes (32-bits))
 */
extern uint32_t _flash_sdata;
extern uint32_t _sram_sdata;
extern uint32_t _sram_edata;
extern uint32_t _sram_sbss;
extern uint32_t _sram_ebss;
extern uint32_t _sram_stacktop;

extern void main(void);
extern void uart0_irq_handler(void);

/* This is an unused handler that simply loops infinitely
 * using the __attribute__ ((weak, alias("function_name")))
 * we'll point all hitherto unused exception handlers to execute
 * this function
 */
void _Unused_Handler(void)
{
    while(1);
}

/* Declaration of exception handlers
 * based on the exception table
 * Refer: http://www.ti.com/lit/ds/symlink/lm3s6965.pdf Table 2-8
 */
void _Reset_Handler(void);
void _NMI_Handler(void)             __attribute__ ((weak, alias("_Unused_Handler")));
void _Hard_Fault_Handler(void)      __attribute__ ((weak, alias("_Unused_Handler")));
void _Memory_Mgmt_Handler(void)     __attribute__ ((weak, alias("_Unused_Handler")));
void _Bus_Fault_Handler(void)       __attribute__ ((weak, alias("_Unused_Handler")));
void _Usage_Fault_Handler(void)     __attribute__ ((weak, alias("_Unused_Handler")));
void _SVCall_Handler(void)          __attribute__ ((weak, alias("_Unused_Handler")));
void _Debug_Monitor_Handler(void)   __attribute__ ((weak, alias("_Unused_Handler")));
void _PendSV_Handler(void)          __attribute__ ((weak, alias("_Unused_Handler")));
void _SysTick_Handler(void)         __attribute__ ((weak, alias("_Unused_Handler")));
void dflt_irq_handler(void)         __attribute__ ((weak, alias("_Unused_Handler")));

/* Exception Table */
__attribute__ ((section(".vectors"), used))
void (* const  _exceptions[])(void) = {
     (void (*)(void))((uint32_t)&_sram_stacktop),           // 00: Reset/Initial Main Stack Pointer
    _Reset_Handler,                                         // 01: Initial Program Counter
    _NMI_Handler,                                           // 02: Non-Maskable Interrupt(NMI)
    _Hard_Fault_Handler,                                    // 03: Hard Fault
    _Memory_Mgmt_Handler,                                   // 04: Memory Management Fault
    _Bus_Fault_Handler,                                     // 05: Bus Fault
    _Usage_Fault_Handler,                                   // 06: Usage Fault
    0,                                                      // 07: Reserved
    0,                                                      // 08: Reserved
    0,                                                      // 09: Reserved
    0,                                                      // 10: Reserved
    _SVCall_Handler,                                        // 11: Supervisor Call
    _Debug_Monitor_Handler,                                 // 12: Debug Monitor
    0,                                                      // 13: Reserved
    _PendSV_Handler,                                        // 14: Pendable req serv
    _SysTick_Handler,                                       // 15: System timer tick
    
    dflt_irq_handler,                                       // 16:GPIO Port A
    dflt_irq_handler,                                       // 17:GPIO Port B
    dflt_irq_handler,                                       // 18:GPIO Port C
    dflt_irq_handler,                                       // 19:GPIO Port D
    dflt_irq_handler,                                       // 20:GPIO Port E
    uart0_irq_handler,                                      // 21: UART0
    dflt_irq_handler,                                       // 22: UART1
    dflt_irq_handler,                                       // 23: SSI0
    dflt_irq_handler,                                       // 24: SSI0
    dflt_irq_handler,                                       // 25: PWM Fault
    dflt_irq_handler,                                       // 26: PWM Generator 0
    dflt_irq_handler,                                       // 27: PWM Generator 1
    dflt_irq_handler,                                       // 28: PWM Generator 2
    dflt_irq_handler,                                       // 29: QEI0
    dflt_irq_handler,                                       // 30: ADC0 Sequence 0
    dflt_irq_handler,                                       // 31: ADC0 Sequence 1
    dflt_irq_handler,                                       // 32: ADC0 Sequence 2
    dflt_irq_handler,                                       // 33: ADC0 Sequence 3
    dflt_irq_handler,                                       // 34: Watchdog Timer 0
    dflt_irq_handler,                                       // 35: Timer 0A
    dflt_irq_handler,                                       // 36: Timer 0B
    dflt_irq_handler,                                       // 37: Timer 1A
    dflt_irq_handler,                                       // 38: Timer 1B
    dflt_irq_handler,                                       // 39: Timer 2A
    dflt_irq_handler,                                       // 40: Timer 2B
    dflt_irq_handler,                                       // 41: Analog Comparator 0
    dflt_irq_handler,                                       // 42: Analog Comparator 1
    0,                                                      // 43: Reserved
    dflt_irq_handler,                                       // 44: System Control
    dflt_irq_handler,                                       // 45: Flash Memory Control
    dflt_irq_handler,                                       // 46: GPIO Port F
    dflt_irq_handler,                                       // 47: GPIO Port G
    0,                                                      // 48: Reserved
    dflt_irq_handler,                                       // 49: UART2
    0,                                                      // 48: Reserved
    dflt_irq_handler,                                       // 51: Timer 3A
    dflt_irq_handler,                                       // 52: Timer 3B
    dflt_irq_handler,                                       // 53: I2C1
    dflt_irq_handler,                                       // 54: QEI1
    0,                                                      // 55: Reserved
    0,                                                      // 56: Reserved
    0,                                                      // 57: Reserved
    dflt_irq_handler,                                       // 58: Ethernet Controller
    dflt_irq_handler,                                       // 59: Hibernation Module
};

/* _Reset_Handler is what is invoked when the processor is reset.
 * As seen in the vector table, it represents the initial value of
 * the program counter. This is where we setup and call main()
 * We'll create a separate section .startup so this resides 
 * immediately after the vector table - as required by LM3S6965 (ARM Cortex-M3)
 */
__attribute__ ((section(".startup")))
void _Reset_Handler(void)
{
    /* Let's disable interrupts for now, shall we */
    irq_master_disable();

    /* Copy the data segment from flash to sram */

    uint32_t *pSrc = &_flash_sdata;   
    uint32_t *pDest = &_sram_sdata;

    while(pDest < &_sram_edata)
    {
        *pDest++ = *pSrc++;
    }

    /* Zero initialize the bss segment in sram */
    pDest = &_sram_sbss;
    
    while(pDest < &_sram_ebss)
    {
        *pDest++ = 0;
    }

    /* Call main() */
    main();

   /* main() isn't supposed return 
    * - if it does, we need to identify this
    * for now, we'll loop infintely
    */
    while(1);
}

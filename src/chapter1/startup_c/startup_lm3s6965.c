#include <stdint.h>
#define CORTEX_M3_EXCEPTIONS 16

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

/* Exception Table */
__attribute__ ((section(".vectors"), used))
void (* const  _exceptions[CORTEX_M3_EXCEPTIONS])(void) = {
     (void (*)(void))((uint32_t)&_sram_stacktop),           // 00: Reset value of the Main Stack Pointer
    _Reset_Handler,                                         // 01: Reset value of the Program Counter
    _NMI_Handler,                                           // 02: Non-Maskable Interrupt (NMI)
    _Hard_Fault_Handler,                                    // 03: Hard Fault
    _Memory_Mgmt_Handler,                                   // 04: Memory Management Fault
    _Bus_Fault_Handler,                                     // 05: Bus Fault
    _Usage_Fault_Handler,                                   // 06: Usage Fault
    _Unused_Handler,                                        // 07: --
    _Unused_Handler,                                        // 08: --
    _Unused_Handler,                                        // 09: --
    _Unused_Handler,                                        // 10: --
    _SVCall_Handler,                                        // 11: Supervisor Call
    _Debug_Monitor_Handler,                                 // 12: Debug Monitor
    _Unused_Handler,                                        // 13: --
    _PendSV_Handler,                                        // 14: Pendable req serv
    _SysTick_Handler,                                       // 15: System timer tick
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
    /* Copy the data segment from flash to sram */
    uint32_t *pSrc = &_flash_sdata;   
    uint32_t *pDest = &_sram_sdata;

    while(pDest < &_sram_edata)
    {
        *pDest = *pSrc;
        pDest++;
        pSrc++;
    }

    /* Zero initialize the bss segment in sram */
    pDest = &_sram_sbss;
    
    while(pDest < &_sram_ebss)
    {
        *pDest = 0;
        pDest++;
    }

    /* Call main() */
    main();

   /* main() isn't supposed return 
    * - if it does, we need to identify this
    * for now, we'll loop infintely
    */
    while(1);
}


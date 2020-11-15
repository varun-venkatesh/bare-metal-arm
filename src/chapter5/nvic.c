#include <stdint.h>
#include "lm3s6965_memmap.h"
#include "nvic.h"

#define NVIC_BASE       ((M3_PERIPHERAL_BASE)+ 0x00000100u)

/* NVIC register map structure.
 * Refer: http://www.ti.com/lit/ds/symlink/lm3s6965.pdf Table 3-7.
 * Note: all offsets in comments are from NVIC_BASE == 0xE000E100
 */
typedef struct __attribute__ ((packed)){
    uint32_t EN0;                   // 0x00 Interrupt 0-31 Set Enable 
    uint32_t EN1;                   // 0x04 Interrupt 32-43 Set Enable
    uint32_t reserved[30];          // 0x08-080 reserved
    uint32_t DIS0;                  // 0x80 Interrupt 0-31 Set Disable 
    uint32_t DIS1;                  // 0x84 Interrupt 32-43 Set Disable
    uint32_t reserved1[30];         // 0x88-100 reserved
    uint32_t PEND0;                 // 0x100 Interrupt 0-31 Set Pending 
    uint32_t PEND1;                 // 0x104 Interrupt 32-43 Set Pending
    uint32_t reserved2[30];         // 0x108-180 reserved
    uint32_t UNPEND0;               // 0x180 Interrupt 0-31 Clear Pending 
    uint32_t UNPEND1;               // 0x184 Interrupt 32-43 Clear Pending
    uint32_t reserved3[30];         // 0x188-200 reserved
    uint32_t ACTIVE0;               // 0x200 Interrupt 0-31 Active Bit
    uint32_t ACTIVE1;               // 0x204 Interrupt 32-43 Active Bit
    uint32_t reserved4[62];         // 0x208-300 reserved
    uint32_t PRI0;                  // 0x300 Interrupt 0-3 Priority
    uint32_t PRI1;                  // 0x304 Interrupt 4-7 Priority
    uint32_t PRI2;                  // 0x308 Interrupt 8-11 Priority
    uint32_t PRI3;                  // 0x30C Interrupt 12-15 Priority
    uint32_t PRI4;                  // 0x310 Interrupt 16-19 Priority
    uint32_t PRI5;                  // 0x314 Interrupt 20-23 Priority
    uint32_t PRI6;                  // 0x318 Interrupt 24-27 Priority
    uint32_t PRI7;                  // 0x31C Interrupt 28-31 Priority
    uint32_t PRI8;                  // 0x320 Interrupt 32-35 Priority
    uint32_t PRI9;                  // 0x324 Interrupt 36-39 Priority
    uint32_t PRI10;                 // 0x328 Interrupt 40-43 Priority
}nvic_regs;

static volatile nvic_regs *nvic = (nvic_regs*)NVIC_BASE;

/* Enables the interrupt specified via
 * param vector_num. 
 * See: http://www.ti.com/lit/ds/symlink/lm3s6965.pdf Section 3.4 (Page 109)
 */
void nvic_irq_enable(uint32_t vector_num)
{
    /* Interrupts 0-31 enabled via EN0 */
    if(vector_num >= IRQ_GPIOA && vector_num <= IRQ_ADC0_SEQ1) 
    {
        nvic->EN0 |= (1 << (vector_num - IRQ_GPIOA));
    }
    /* Interrupts 31-43 enabled via EN1 */
    else if(vector_num >= IRQ_ADC0_SEQ2 && vector_num <= IRQ_HIBERNATE) 
    {
        nvic->EN1 |= (1 << (vector_num - IRQ_ADC0_SEQ2));
    }

}

/* Disables the interrupt specified via
 * param vector_num. 
 * See: http://www.ti.com/lit/ds/symlink/lm3s6965.pdf Section 3.4 (Page 109)
 */
void nvic_irq_disable(uint32_t vector_num)
{
    /* Interrupts 0-31 disabled via DIS0 */
    if(vector_num >= IRQ_GPIOA && vector_num <= IRQ_ADC0_SEQ1) 
    {
        nvic->DIS0 |= (1 << (vector_num - IRQ_GPIOA));
    }
    /* Interrupts 31-43 disabled via DIS1 */
    else if(vector_num >= IRQ_ADC0_SEQ2 && vector_num <= IRQ_HIBERNATE) 
    {
        nvic->DIS1 |= (1 << (vector_num - IRQ_ADC0_SEQ2));
    }
}

#include <stdint.h>
#include "lm3s6965_memmap.h"
#include "nvic.h"

/* NVIC register map structure.
 * Refer: http://www.ti.com/lit/ds/symlink/lm3s6965.pdf Table 3-7.
 * Note: all offsets in comments are from NVIC_BASE == 0xE000E100
 */
typedef struct __attribute__ ((packed)){
    uint32_t EN0;                   // 0x00 Interrupt 0-31 Set Enable 
    uint32_t EN1;                   // 0x04 Interrupt 32-43 Set Enable
    uint32_t reserved[30];          // 0x08-080 reserved
    uint32_t DIS0;                  // 0x80 Interrupt 0-31 Set Disable 
    uint32_t DIS1;                  // 0x84 Interrupt 0-31 Set Disable
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

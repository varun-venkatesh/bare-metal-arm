#include <stdint.h>
#include "lm3s6965_memmap.h"
#include "sysctl.h"
#include "systick.h"
#include "uart_drv.h"
#include "serial_print.h"

#define SYS_TIMER_BASE          ((M3_PERIPHERAL_BASE)+ 0x00000010u)
#define MILLISECS_IN_SEC        1000u
#define TWENTY_TICKS            20u

/* System Timer SysTcik register map structure.
 * Refer: http://www.ti.com/lit/ds/symlink/lm3s6965.pdf Table 3-7 and Section 3-3
 */
typedef struct __attribute__((packed)){
    uint32_t STCTRL;            //0x0 SysTick Control and Status Register
    uint32_t STRELOAD;          //0x4 SysTick Reload Value Register
    uint32_t STCURRENT;         //0x8 SysTick Current Value Register
}systick_regs;

static volatile systick_regs *systick = (systick_regs*)SYS_TIMER_BASE;

// count variable to maintain the number of times the Systick counter counted down to zero.
static volatile uint32_t tick_count = 0;

/* Function to enable the SysTick timer 
 * by setting the ENABLE bit and 
 * selecting the apporpriate clk source - in this case System Clock
 */
void systick_enable(void)
{
    systick->STCTRL |= STCTRL_CLKSRC | STCTRL_ENABLE;
}

/* Function to enable the SysTick timer 
 * by clearing the ENABLE bit and 
 */
void systick_disable(void)
{
    systick->STCTRL &= ~(STCTRL_ENABLE);

}

/* Function to enable the SysTick timer interrupt 
 * by setting the INTEN bit and 
 */
void systick_irq_enable(void)
{
    systick->STCTRL |= STCTRL_INTEN; 
}

/* Function to disable the SysTick timer interrupt 
 * by clearing the INTEN bit and 
 */
void systick_irq_disable(void)
{
    systick->STCTRL &= ~(STCTRL_INTEN);
}

/* Utility function to convert the
 * the time period in milliseconds to
 * the timer period based on the system clock frquency
 */
static uint32_t systick_millisec_to_timer_period(uint32_t millisec)
{
    /* this scheme of dividing the clock frequency by 1000
     * to calculate cycles per millisec rather than multiply clock frequency
     * with millisec and divide by 1000 - is to avoid the overflow in the latter case.
     */
    uint32_t period = (sysctl_getclk()/1000u) * millisec;
    return period;

}

/* Function to use the time period in milliseconds provided 
 * as the SysTick countdown value -by first converting the
 * period in milliseconds to the timer period.
 */
void systick_set_period_ms(uint32_t millisec)
{
    /* TODO: check to ensure that the period is 
     * no greater than 2^24 (- uses 24 bits)
     */
    uint32_t count = systick_millisec_to_timer_period(millisec);
    systick->STRELOAD = count;
}

/* The SysTick interrupt handler - currently configured to print 
 * the number of Systick ticks elapsed at the rate of every 20 ticks
 */
void _SysTick_Handler(void)
{
    tick_count++;
    if(tick_count % 20u == 0)
    {
        serial_put_uint(tick_count);
        serial_puts(" time ticks have elapsed!\n");
    }
}


#include <stdint.h>
#include "irq.h"
#include "nvic.h"
#include "sysctl.h"
#include "systick.h"
#include "uart_drv.h"
#include "serial_print.h"

/* main() represents the entry point in a c program.
 * In this bare-metal system, main represents the 
 * function where we initialize the various peripherals and 
 * in a way serves as an entry point to the (initialized) system
 */

int main(void)
{
    const char *start_msg = "Hello, World\n";
    uint32_t clk_cfg1, clk_cfg2;

    /* Let's now re-enable the interrupts*/
    irq_master_enable();

    /* Also, let's also turn on the UART0 interrupt */
    nvic_irq_enable(IRQ_UART0);

    /* Set the system clock to the PLL with the main oscillator as the source
     * with the crystal frequency set to 8 MHz. 
     * Divide the PLL output clock frquency by a factor of 12.
     * Turn off the (unused) internal oscillator. This is to configure a system clock of 16.67 MHz.
     */
    clk_cfg1 = (SYSCTL_PLL_SYSCLK | SYSCTL_RCC_USESYSDIV | SYSCTL_RCC_SYSDIV_11 | 
               SYSCTL_RCC_XTAL_8MHZ | SYSCTL_RCC_OSCSRC_MOSC | SYSCTL_RCC_IOSCDIS);
    clk_cfg2 = 0;

    sysctl_setclk(clk_cfg1, clk_cfg2);
    
    /* Let's set systick period to be 0.5 seconds =>
     * system clock frequency divided by 2.
     */
    systick_set_period_ms(500u);

    /* Let's enable the systick timer and it's interrupt */
    systick_irq_enable();
    systick_enable();

    /* Configure the uart to a baud-rate of 115200 */
    uart_init(UART_BAUD_115200);

    serial_puts(start_msg);

    serial_putchar('.');
    serial_putchar('.');
    serial_putchar('.');

    serial_puts("\nGo on, say something...\n");

    while(1);

    return 0;
}

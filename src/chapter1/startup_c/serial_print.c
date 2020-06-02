#include <stdint.h>

/* We'll be using the serial interface to print on the console (qemu)
 * For this, we write to the UART. 
 * Refer: http://www.ti.com/lit/ds/symlink/lm3s6965.pdf Table 2-8
 * for the mapping of UART devices (including UART0)
 */

// #define LM3S6965_UART0 (*(volatile uint8_t *)0x4000C000)

volatile uint8_t* lm3s6965_uart0 = (uint8_t*)0x4000C000;

void uart0_print(const char* msg);

void main(void)
{
    const char *start_msg = "Hello, World\n";

    uart0_print(start_msg);
    
    while(1);
}

void uart0_print(const char* msg)
{
    while(*msg)
    {
        *lm3s6965_uart0 = *msg;
        msg++;
    }
}

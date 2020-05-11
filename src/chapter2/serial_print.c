#include "uart_drv.h"

/* Output a character */
void serial_putchar(const char c)
{
    uart_tx_byte(c);
}

/* Read a character */
uart_err serial_getchar(char* pc)
{
    uart_err retval;
    retval = uart_rx_byte(pc);
    return retval;
}


/* Output a string */
void serial_puts(const char* str)
{
    while(*str)
    {
        serial_putchar(*str);
        str++;
    }
}

/* Read a string into a buffer */
void serial_gets(char* str)
{
    char c;
    char *pstr = str;

    while(1)
    {
        if(serial_getchar(&c) == UART_OK)
        {
            if(c == '\r')
            {
                *pstr = '\0';
                break;
            }
            else
            {
                *pstr = c;
            }
            
            pstr++;
        }
    }
}

int main(void)
{
    const char *start_msg = "Hello, World\n";
    char first_ip[64];

    uart_init(115200);

    serial_puts(start_msg);

    serial_putchar('.');
    serial_putchar('.');
    serial_putchar('.');

    serial_puts("\nGo on, say something...\n");

    while(1)
    {
        serial_gets(first_ip);

        serial_puts("You said:\t");
        serial_puts(first_ip);
        serial_putchar('\n');
    }

    return 0;
}


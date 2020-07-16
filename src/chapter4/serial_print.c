#include <stdint.h>
#include "uart_drv.h"
#include "serial_print.h"

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

/* Output an integer passed to this function */
void serial_put_uint(uint32_t num)
{
    /* the buffer that holds the string representation of the number.
     * max value of an unsigned 32-bit integer == 4294967295 - 10-digits
     * i.e. a 10 character string isrequired to hold it
     */
    char num_buf[10] = {0};
    uint8_t idx = 0;
    uint8_t remainder;

    // using do-while to account for num = 0 - which still needs printing.
    do
    {
        remainder = num % 10;
        num_buf[idx] = '0' + remainder;
        idx++;
        num /= 10;
    }while(num != 0);

    while(idx)
    {
        serial_putchar(num_buf[idx-1]);
        idx--;
    }

}

#ifndef __SERIAL_PRINT_H__
#define __SERIAL_PRINT_H__

void serial_putchar(const char c);
uart_err serial_getchar(char* pc);
void serial_puts(const char* str);
void serial_gets(char* str);
void serial_put_uint(uint32_t num);

#endif /*  __SERIAL_PRINT_H__ */

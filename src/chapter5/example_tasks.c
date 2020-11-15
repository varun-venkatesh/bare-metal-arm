#include <stdio.h>
#include "example_tasks.h"
#include "system_time.h"
#include "uart_drv.h"
#include "serial_print.h"

void example_task0(void)
{
    systime_t entry_time = system_time_get();
    serial_puts("example_task0 entered system_time: ");
    serial_put_uint(entry_time);
    serial_putchar('\n');
    while(entry_time + 1000u > system_time_get()){

    }
    serial_puts("example_task0 exits!\n");
}

void example_task1(void)
{
    systime_t entry_time = system_time_get();
    serial_puts("example_task1 entered system_time: ");
    serial_put_uint(entry_time);
    serial_putchar('\n');
    while(entry_time + 1000u > system_time_get()){

    }
    serial_puts("example_task1 exits!\n");
}

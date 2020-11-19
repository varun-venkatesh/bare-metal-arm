#include <stdio.h>
#include "example_tasks.h"
#include "system_time.h"
#include "uart_drv.h"
#include "serial_print.h"

/* Our example tasks don't do much other than:
 * make note of the entry time in terms of systime_t and print this on the console
 * loop until the task has run for 1000 systeim_t units since it entered
 * print an exit message and return.
 */

void example_task0(void) {
    systime_t entry_time = system_time_get();
    serial_puts("example_task0 entered system_time: ");
    serial_put_uint(entry_time);
    serial_putchar('\n');
    while(entry_time + 1000u > system_time_get()) ;
    serial_puts("example_task0 exits!\n");
}

void example_task1(void) {
    systime_t entry_time = system_time_get();
    serial_puts("example_task1 entered system_time: ");
    serial_put_uint(entry_time);
    serial_putchar('\n');
    while(entry_time + 1000u > system_time_get());
    serial_puts("example_task1 exits!\n");
}

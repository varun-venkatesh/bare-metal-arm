#include "system_time.h"

/* To keep track of the time across
 * our system since startup.
 */
static volatile systime_t system_time;

/* Increment the system time as and when the 
 * predefined SysTick period set in the init elapses.
 */
void system_time_incr(void){
    system_time++;
    return;
}

/* Return the current system time value 
 */
systime_t system_time_get(void){
    return system_time;
}

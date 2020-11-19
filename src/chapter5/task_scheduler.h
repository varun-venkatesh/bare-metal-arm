#include "system_time.h"

#define MAX_TASKS   (10u)

/* Defining a function pointer type for a task's start function/routine */
typedef void (*task_start_fptr)(void);

/* Task descriptor that includes
 * a pointer to the entry function of the task
 * the duration of the task in systime_t units
 * the last systime_t when the task was run
 */
typedef struct{
    task_start_fptr     start;
    systime_t           duration;
    systime_t           last_run;
}task_desc;

/* Error enumerations for the scheduler */
typedef enum{
    SCHEDULER_OKAY = 0,
    SCHEDULER_TOO_MANY_TASKS
}task_scheduler_err;

task_scheduler_err task_scheduler_add_task(task_start_fptr start, systime_t duration);
void task_scheduler_run(void);


#include "system_time.h"

#define MAX_TASKS   (10u)

/* Defining a function pointer type for a task's start function/routine */
typedef void (*task_start_fptr)(void);

typedef struct{
    task_start_fptr     start;
    systime_t           duration;
    systime_t           last_run;
}task_desc;

typedef enum{
    SCHEDULER_OKAY = 0,
    SCHEDULER_TOO_MANY_TASKS
}task_scheduler_err;

task_scheduler_err task_scheduler_add_task(task_start_fptr start, systime_t duration);
void task_scheduler_run(void);


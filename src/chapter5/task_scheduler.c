#include <stdio.h>
#include "task_scheduler.h"

static task_desc task_list[MAX_TASKS] = {0};
static uint8_t task_list_idx;

task_scheduler_err task_scheduler_add_task(task_start_fptr start, systime_t duration){
    task_desc new_task;

    if(task_list_idx >= MAX_TASKS)
    {
        return SCHEDULER_TOO_MANY_TASKS;
    }

    new_task.start = start;
    new_task.duration = duration;
    new_task.last_run = 0;

    task_list[task_list_idx] = new_task;
    task_list_idx++;

    return SCHEDULER_OKAY;
}

void task_scheduler_run(void){
    uint8_t idx;
    while(1){
        for(idx = 0; idx < MAX_TASKS; idx++){
            task_desc* curr_task = &task_list[idx];
            
            if(curr_task->start == NULL){
                continue;
            }

            if(system_time_get() - curr_task->last_run >= curr_task->duration){
                curr_task->last_run = system_time_get();
                curr_task->start();
            }
        }
    }
}

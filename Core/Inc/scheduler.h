#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <stdint.h>
#include <stdbool.h>

#define MAX_TASKS 10
#define TIMER_TICK_US 21 //as measured by scope

typedef void (*task_func_t)(void);

typedef struct {
    task_func_t function;
    uint32_t interval_us;
    uint32_t next_execution;
    uint8_t remaining_runs;
    bool active;
    bool lies_in_future_time_frame;
} Task;

void scheduler_init(void);
void scheduler_add(task_func_t func, uint32_t delay_us, uint8_t run_count);
void scheduler_remove(task_func_t func);
void scheduler_run(void);
void scheduler_tick(void);

#endif // SCHEDULER_H

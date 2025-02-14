#include "scheduler.h"
#include <stdio.h>

volatile Task task_list[MAX_TASKS];
volatile uint16_t current_time_us = 0;

void scheduler_init(void) {
    for (uint8_t i = 0; i < MAX_TASKS; i++) {
        task_list[i].active = false;
    }
}

void scheduler_add(task_func_t func, uint16_t delay_us, uint8_t run_count) {
    for (uint8_t i = 0; i < MAX_TASKS; i++) {
        if (!task_list[i].active) {
            task_list[i].function = func;
            task_list[i].interval_us = delay_us;
            task_list[i].next_execution = current_time_us + delay_us;
            task_list[i].remaining_runs = (run_count == 0) ? 255 : run_count; // 255 represents infinite execution
            task_list[i].active = true;
            return;
        }
    }
}

void scheduler_remove(task_func_t func) {
    for (uint8_t i = 0; i < MAX_TASKS; i++) {
        if (task_list[i].active && task_list[i].function == func) {
            task_list[i].active = false;
        }
    }
}

void scheduler_run(void) {
    for (uint8_t i = 0; i < MAX_TASKS; i++) {
        if (task_list[i].active && (int16_t)(current_time_us - task_list[i].next_execution) >= 0) {
            task_list[i].function();
            if (task_list[i].remaining_runs > 1 || task_list[i].remaining_runs == 255) {
                if (task_list[i].remaining_runs != 255) {
                    task_list[i].remaining_runs--;
                }
                task_list[i].next_execution += task_list[i].interval_us;
            } else {
                task_list[i].active = false;
            }
        }
    }
}

// ISR called every 5 microseconds
void scheduler_tick(void) {
    current_time_us += TIMER_TICK_US;
}

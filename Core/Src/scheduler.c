#include "scheduler.h"
#include <stdio.h>
#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_tim.h"
#include "stm32f1xx_hal_gpio.h"

volatile Task task_list[MAX_TASKS];
volatile uint16_t current_time_us = 0;


//changes next_execution-time and sets lies_in_future-flag. No new task is added.
void scheduler_calc_next_execution(Task* task, uint16_t delay_us){
	task->next_execution = current_time_us + delay_us;
	task->lies_in_future_time_frame = task->next_execution < current_time_us;
}



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
            scheduler_calc_next_execution(&(task_list[i]), delay_us);
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
        if (task_list[i].active && (int16_t)(current_time_us - task_list[i].next_execution) >= 0 && !task_list[i].lies_in_future_time_frame) {
            task_list[i].function();
            if (task_list[i].remaining_runs > 1 || task_list[i].remaining_runs == 255) {
                if (task_list[i].remaining_runs != 255) {
                    task_list[i].remaining_runs--;
                }
                scheduler_calc_next_execution(&task_list[i], task_list[i].interval_us);
            } else {
                task_list[i].active = false;
            }
        }
    }
}

// ISR called every 5 microseconds
void scheduler_tick(void) {
	HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_15);
	uint16_t prev_time = current_time_us;
    current_time_us += TIMER_TICK_US;
    if(current_time_us < prev_time){
    	for(uint8_t i = 0; i < MAX_TASKS; i++){
    		task_list[i].lies_in_future_time_frame = false;
    	}
    }

}

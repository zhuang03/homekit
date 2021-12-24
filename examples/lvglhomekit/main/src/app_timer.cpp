/* General Purpose Timer example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/timer.h"




#include "app_timer.h"


timer_signal timer;

AppTimer::AppTimer(/* args */)
{
}

AppTimer::~AppTimer()
{
}





static bool IRAM_ATTR timer_group_isr_callback(void *args)
{


     BaseType_t high_task_awoken = pdFALSE;
     
    // example_timer_info_t *info = (example_timer_info_t *) args;

    // uint64_t timer_counter_value = timer_group_get_counter_value_in_isr(info->timer_group, info->timer_idx);

    // /* Prepare basic event data that will be then sent back to task */
    // example_timer_event_t evt = {
    //     .info.timer_group = info->timer_group,
    //     .info.timer_idx = info->timer_idx,
    //     .info.auto_reload = info->auto_reload,
    //     .info.alarm_interval = info->alarm_interval,
    //     .timer_counter_value = timer_counter_value
    // };

    // if (!info->auto_reload) {
    //     timer_counter_value += info->alarm_interval * TIMER_SCALE;
    //     timer_group_set_alarm_value_in_isr(info->timer_group, info->timer_idx, timer_counter_value);
    // }

    // timer_group_clr_intr_status_in_isr(info->timer_group,info->timer_idx);
    /* Now just send the event data back to the main program task */
    // xQueueSendFromISR(s_timer_queue, &evt, &high_task_awoken);

    timer.second++;




    return high_task_awoken == pdTRUE; // return whether we need to yield at the end of ISR
}

/**
 * @brief Initialize selected timer of timer group
 *
 * @param group Timer Group number, index from 0
 * @param timer timer ID, index from 0
 * @param auto_reload whether auto-reload on alarm event
 * @param timer_interval_sec interval of alarm
 */
void AppTimer::timer_set_init(timer_group_t group, timer_idx_t timer, timer_autoreload_t auto_reload, int timer_interval_sec)
{
    /* Select and initialize basic parameters of the timer */

    config.divider = TIMER_DIVIDER,
    config.counter_dir = TIMER_COUNT_UP,
    config.counter_en = TIMER_PAUSE,
    config.alarm_en = TIMER_ALARM_EN,
    config.auto_reload = auto_reload,
     // default clock source is APB
    timer_init(group, timer, &config);   // default clock source is APB

    /* Timer's counter will initially start from value below.
       Also, if auto_reload is set, this value will be automatically reload on alarm */
    timer_set_counter_value(group, timer, 0);

    /* Configure the alarm value and the interrupt on alarm. */
    timer_set_alarm_value(group, timer, timer_interval_sec * TIMER_SCALE); 
    timer_enable_intr(group, timer);

    example_timer_info_t *timer_info = (example_timer_info_t *)calloc(1, sizeof(example_timer_info_t));

    timer_info->timer_group = group;
    timer_info->timer_idx = timer;
    timer_info->auto_reload = auto_reload;
    timer_info->alarm_interval = timer_interval_sec;
    timer_isr_callback_add(group, timer, timer_group_isr_callback, timer_info, 0);

    timer_start(group, timer);
}


void AppTimer::init(void)
{
   
  timer_set_init(TIMER_GROUP_0, TIMER_0, TIMER_AUTORELOAD_EN, 1);   /*定时1s*/
 // example_tg_timer_init(TIMER_GROUP_1, TIMER_0, false, 5);
}


// void AppTimer::Read_Timer(uint8_t second)
// {
   


// }




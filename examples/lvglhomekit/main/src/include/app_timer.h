#ifndef  _APP_TIMER_H_
#define _APP_TIMER_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "driver/timer.h"

#define TIMER_DIVIDER         (16)  //  Hardware timer clock divider
#define TIMER_SCALE           (TIMER_BASE_CLK / TIMER_DIVIDER)  // convert counter value to seconds

typedef struct {
    int timer_group;
    int timer_idx;
    int alarm_interval;
    bool auto_reload;
} example_timer_info_t;

/**
 * @brief A sample structure to pass events from the timer ISR to task
 *
 */
typedef struct {
    example_timer_info_t info;
    uint64_t timer_counter_value;
} example_timer_event_t;


/**
 * @brief timer siangl 
 *
 */
typedef struct {
    uint64_t hour;
    uint64_t minute;
    uint64_t second;
} timer_signal;




class AppTimer
{
private:
   timer_config_t config;
   void timer_set_init(timer_group_t group, timer_idx_t timer, timer_autoreload_t auto_reload, int timer_interval_sec);
public:
    void init(void);
    AppTimer();
    ~AppTimer();
};





#endif // ! _APP_TIMER_H_
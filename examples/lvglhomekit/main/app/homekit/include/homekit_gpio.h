#ifndef _HOMEKIT_GPIO_H_
#define _HOMEKIT_GPIO_H_

#ifdef __cplusplus
extern "C" {
#endif
 
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "driver/gpio.h"


#define GPIO_OUTPUT_IO_0    18
#define GPIO_OUTPUT_IO_1    19
#define GPIO_OUTPUT_PIN_SEL  ((1ULL<<GPIO_OUTPUT_IO_0) | (1ULL<<GPIO_OUTPUT_IO_1))

#define GPIO_INPUT_IO_0     16
#define GPIO_INPUT_IO_1     17
#define GPIO_INPUT_PIN_SEL  ((1ULL<<GPIO_INPUT_IO_0) | (1ULL<<GPIO_INPUT_IO_1))
#define ESP_INTR_FLAG_DEFAULT  0



void init_gpio(void);

void ferris_wheel_on_off(bool value);

void electric_light_on_off(bool value);




#ifdef __cplusplus
} /* extern "C" */
#endif




#endif


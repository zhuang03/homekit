/* GPIO Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "driver/gpio.h"



#include <esp_log.h>
#include "homekit_gpio.h"

static const char *TAG = "Bridge";

/*外部中断回调函数*/
static void IRAM_ATTR gpio_isr_handler(void* arg)
{

 if(gpio_get_level(GPIO_INTR_POSEDGE)==0){

  }
}




void init_gpio(void)
{

   gpio_config_t io_conf;
  
   io_conf.intr_type = GPIO_INTR_DISABLE;       /*disable interrupt*/
   io_conf.mode = GPIO_MODE_OUTPUT;             /*set as output mode*/
   io_conf.pin_bit_mask = GPIO_OUTPUT_PIN_SEL;  /*bit mask of the pins that you want to set,e.g.GPIO18/19*/
   io_conf.pull_down_en = 0;                    /*disable pull-down mode*/
   io_conf.pull_up_en = 0;                      /*disable pull-up mode*/
   gpio_config(&io_conf);                       /*configure GPIO with the given settings*/
   
   gpio_set_level(GPIO_OUTPUT_IO_0, 0);
   gpio_set_level(GPIO_OUTPUT_IO_1, 0);     


    // /*外部中断引脚配置*/
    // io_conf.intr_type = GPIO_INTR_POSEDGE;       /*interrupt of rising edge*/
    // io_conf.mode = GPIO_MODE_INPUT;              /*set as input mode*/
    // io_conf.pin_bit_mask = GPIO_INPUT_PIN_SEL;   /*bit mask of the pins, use GPIO4/5 here*/
    // io_conf.pull_up_en = 1;                      /*enable pull-up mode*/
    // gpio_config(&io_conf);

    // //  gpio_set_intr_type(GPIO_INPUT_IO_0, GPIO_INTR_ANYEDGE);    /*change gpio intrrupt type for one pin*/

    // gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);  /*install gpio isr service*/
    // gpio_isr_handler_add(GPIO_INTR_POSEDGE, gpio_isr_handler, (void*)GPIO_INTR_POSEDGE); /*hook isr handler for specific gpio pin*/

    // // gpio_isr_handler_add(GPIO_INPUT_IO_1, gpio_isr_handler, (void*) GPIO_INPUT_IO_1); /*hook isr handler for specific gpio pin*/

    // // gpio_isr_handler_remove(GPIO_INPUT_IO_0);    /*remove isr handler for gpio number*/
    // // gpio_isr_handler_add(GPIO_INPUT_IO_0, gpio_isr_handler, (void*) GPIO_INPUT_IO_0);  /*hook isr handler for specific gpio pin again*/

}



/*********************开关控制***************************/
void ferris_wheel_on_off(bool value)
{


 ESP_LOGI(TAG, "Received Write ferris wheel %d %s\r\n",value, value ? "On" : "Off");
 gpio_set_level(GPIO_OUTPUT_IO_0,value);

 /*TODO: Control Actual Hardware */
}


void electric_light_on_off(bool value)
{

  ESP_LOGI(TAG, "Received Write electric light %d %s\r\n",value, value ? "On" : "Off");
  /*TODO: Control Actual Hardware */
  gpio_set_level(GPIO_OUTPUT_IO_1,value);

}

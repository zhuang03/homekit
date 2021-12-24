#include <stdio.h>
#include <string.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_log.h>

/*homekit*/
#include "homekit_app.h"
/*lvgl*/
#include "lv_init.h"
#include "lv_sd_fatfs.h"




/*mpu6050*/
#define  TESETING_MPU_SIZE         4*1024
#define  TESETING_MPU_PRIORITY     1
static TaskHandle_t mpu_handles;

/*app*/
#define  APP_STACK_SIZE           4*1024
#define  APP_STACK_PRIORITY       10
static TaskHandle_t app_handles;


Homekit hoit;
LvglInt lvgl;
SdCard sd; 



static void app_task(void *age)
{
  while (1)
  {
    vTaskDelay(100/portTICK_PERIOD_MS); 
  }
  

}




static void mpu_task(void *age)
{
  while (1)
  {
   
    vTaskDelay(100/portTICK_PERIOD_MS); 
  }
  

}


extern "C" void app_main()
{

  hoit.init();    
  hoit.wifi_connect(60*1000); 
  sd.init();   
  lvgl.init();
  lvgl.application();


  xTaskCreatePinnedToCore(app_task,  /*task statck function*/
                          "app_task", /*task statck name*/
                          APP_STACK_SIZE, 
                          NULL,    /*task stack parameter*/
                          APP_STACK_PRIORITY,
                          &app_handles,
                          0);


  xTaskCreatePinnedToCore(mpu_task,  /*task statck function*/
                         "mpu_task", /*task statck name*/
                          TESETING_MPU_SIZE, 
                          NULL,    /*task stack parameter*/
                          TESETING_MPU_PRIORITY,
                          &mpu_handles,
                          0);
}

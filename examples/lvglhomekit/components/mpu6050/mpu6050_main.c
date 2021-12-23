#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_freertos_hooks.h"
#include "freertos/semphr.h"
#include "esp_system.h"


#include "device_i2c.h"
#include "inv_mpu.h"
#include "lvgl_demo.h"
#include "mpu6050_main.h"

#define TAG "mup6050"

#define MPU6050_TASK_RUN_PRIORITY     2
#define MPU6050_TASK_RUN_STACKSIZE    2*1024
#define MPU6050_TASK_RUN_NAME        "mpu6050"

/*MPU6050 任务*/
void mpu6050Task(void *pvParameter) 
{
char anglybuf[100]={0};
uint32_t mpu6050cont=0;

mpu6050_main_t angly;
Init_I2c_Mpu6050();

while(1)
   {
     mpu6050cont++;
     mpu_dmp_get_data(&angly.pitch,&angly.roll,&angly.yaw);
     if(mpu6050cont>2){
        mpu6050cont = 0;
        sprintf(anglybuf,"pitch=%0.2f\r\roll=%0.2f\r\nyaw=%0.2f\r\n",angly.pitch,angly.roll,angly.yaw);
        lvgl_mpu6050_test(anglybuf);
        printf("q21312321\r\n");
     }
    //  printf("ret:%d pitch:%0.2f, roll:%0.2f, yaw:%0.2f\r\n",ret,pitch,roll,yaw); 
     vTaskDelay(pdMS_TO_TICKS(1000));
    }
 vTaskDelete(NULL);
}


void mpu6050_app_main(void) 
{

 /* NOTE: When not using Wi-Fi nor Bluetooth you can pin the guiTask to core 0 */
  xTaskCreatePinnedToCore(mpu6050Task, MPU6050_TASK_RUN_NAME, MPU6050_TASK_RUN_STACKSIZE, NULL, MPU6050_TASK_RUN_PRIORITY, NULL, 0);
}

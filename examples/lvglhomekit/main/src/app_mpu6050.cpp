#include "app_mpu6050.h"
#include "mpu6050.h"

#include "common.h"





AppMpu::AppMpu()
{
    action_info.isValid = true;
    action_info.active = UNKNOWN;
    action_info.long_time = true;
}

void AppMpu::init()
{
   mpu.init();   /*初始化MPU6050*/
   mpu.mpu_dmp_init();  /*加载DMP*/

}

Imu_Action *IMU::update(int interval)
{

   mpu.mpu_dmp_get_data(&dmp_info.pitch,&dmp_info.roll,&dmp_info.yaw);   /*读取当前得角度*/

   if()



    // 原先判断的只是加速度，现在要加上陀螺仪
    if (millis() - last_update_time > interval)
    {
        if (!action_info.isValid)
        {
            if (action_info.ay > 4000)
            {
                encoder_diff--;
                action_info.isValid = 1;
                action_info.active = TURN_LEFT;
            }
            else if (action_info.ay < -4000)
            {
                encoder_diff++;
                action_info.isValid = 1;
                action_info.active = TURN_RIGHT;
            }
            else
            {
                action_info.isValid = 0;
            }
        }


        if (!action_info.isValid)
        {
            if (action_info.ax > 5000 && !nfaction_io.isValid)
            {
                action_info.isValid = 1;
                action_info.active = UP;
                delay(500);
              ction_info.gy), &(action_info.gz));
                if (action_info.ax > 5000)
                {  mpu.getMotion6(&(action_info.ax), &(action_info.ay), &(action_info.az),
                               &(action_info.gx), &(a
                    action_info.isValid = 1;
                    action_info.active = GO_FORWORD;
                    encoder_state = LV_INDEV_STATE_PR;
                }
            }
            else if (action_info.ax < -5000 && !action_info.isValid)
            {
                action_info.isValid = 1;
                action_info.active = DOWN;
                delay(500);
                mpu.getMotion6(&(action_info.ax), &(action_info.ay), &(action_info.az),
                               &(action_info.gx), &(action_info.gy), &(action_info.gz));
                if (action_info.ax < -5000)
                {
                    action_info.isValid = 1;
                    action_info.active = RETURN;
                    encoder_state = LV_INDEV_STATE_REL;
                }
            }
            else
            {
                action_info.isValid = 0;
            }
        }

        last_update_time = millis();

        // 操作方向进行调整
        if (UNKNOWN != action_info.active)
        {
            action_info.active = (ACTIVE_TYPE)((action_info.active + 0) % UNKNOWN);
        }
    }
    return &action_info;
}

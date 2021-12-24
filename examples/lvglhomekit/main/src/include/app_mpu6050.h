#ifndef _APP_MPU6050_H_
#define _APP_MPU6050_H_


#include "lv_port_indev.h"
#include "mpu6050.h"

#define IMU_I2C_SDA 32
#define IMU_I2C_SCL 33

extern int32_t encoder_diff;
extern lv_indev_state_t encoder_state;

enum active_type
{
    TURN_RIGHT = 0,
    RETURN,
    TURN_LEFT,
    UP,
    DOWN,
    GO_FORWORD,
    UNKNOWN
};

struct Imu_Action
{
    active_type active;
    boolean isValid;
    boolean long_time;
    float pitch;
    float roll;
    float yaw;
};


class AppMpu
{
private:
    Mpu6050 mpu;
   int flag;
    long last_update_time;

public:
    Imu_Action dmp_info;

public:
    AppMpu();
    ~mpu();
    void init();
    Imu_Action *update(int interval);
};

#endif

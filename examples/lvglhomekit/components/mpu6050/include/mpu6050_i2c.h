#ifndef _MPU6050_I2C_H_
#define _MPU6050_I2C_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "driver/i2c.h"
#include "sdkconfig.h"
  

#ifdef __cplusplus
extern "C" {
#endif

 
#define _I2C_NUMBER(num) I2C_NUM_##num
#define I2C_NUMBER(num) _I2C_NUMBER(num)


#define DATA_LENGTH                 512    /*!< Data buffer length of test buffer */
#define RW_TEST_LENGTH              128    /*!< Data length for r/w test, [0,DATA_LENGTH] */
#define DELAY_TIME_BETWEEN_ITEMS_MS 1000   /*!< delay time between different test items */


#define I2C_SLAVE_SCL_IO     CONFIG_I2C_SLAVE_SCL                     /*!< gpio number for i2c slave clock */
#define I2C_SLAVE_SDA_IO     CONFIG_I2C_SLAVE_SDA                     /*!< gpio number for i2c slave data */
#define I2C_SLAVE_NUM        I2C_NUMBER(CONFIG_I2C_SLAVE_PORT_NUM)    /*!< I2C port number for slave dev */
#define I2C_SLAVE_TX_BUF_LEN (2 * DATA_LENGTH)                        /*!< I2C slave tx buffer size */
#define I2C_SLAVE_RX_BUF_LEN (2 * DATA_LENGTH)                        /*!< I2C slave rx buffer size */


#define I2C_MASTER_SCL_IO    22                    /*!< gpio number for I2C master clock */
#define I2C_MASTER_SDA_IO    23                    /*!< gpio number for I2C master data  */
#define I2C_MASTER_NUM       I2C_NUMBER(1)         /*!< I2C port number for master dev */
#define I2C_MASTER_FREQ_HZ   400*1000              /*!< I2C master clock frequency */
#define I2C_MASTER_TX_BUF_DISABLE 0                                   /*!< I2C master doesn't need buffer */
#define I2C_MASTER_RX_BUF_DISABLE 0                                   /*!< I2C master doesn't need buffer */



//如果AD0脚(9脚)接地,IIC地址为0X68(不包含最低位).
//如果接V3.3,则IIC地址为0X69(不包含最低位).
#define MPU_ADDR				0X68
// #define MPU_ADDR				0XD0
#define MPU6050_READ            1
#define MPU6050_WRITE          0


#define MPU6060_SENSOR_ADDR  MPU_ADDR                 /*!< slave address for MPU6050 sensor */
#define WRITE_BIT            MPU6050_WRITE           /*!< I2C master write */
#define READ_BIT             MPU6050_READ              /*!< I2C master read */




#define ACK_CHECK_EN         0x1                        /*!< I2C master will check ack from slave*/
#define ACK_CHECK_DIS        0x0                        /*!< I2C master will not check ack from slave */
#define ACK_VAL              0x0                        /*!< I2C ack value */
#define NACK_VAL             0x1                        /*!< I2C nack value */







/*
 * 写入一字节数据
 */
esp_err_t __attribute__((unused)) Mpu6050_i2c_master_write_Byte(i2c_port_t i2c_num,uint8_t adder, uint8_t data_wr);

/*
 * 读取一字节数据
 */
esp_err_t __attribute__((unused)) Mpu6050_i2c_master_read_Byte(i2c_port_t i2c_num, uint8_t adder,uint8_t *data_rd);

/*
 * 写入多字节节数据
 */
esp_err_t __attribute__((unused)) Mpu6050_i2c_master_write_slave(i2c_port_t i2c_num,uint8_t adder,uint8_t *data_wr,size_t size);

/*
 * 读取多字节数据
 */
esp_err_t __attribute__((unused)) Mpu6050_i2c_master_read_slave(i2c_port_t i2c_num, uint8_t adder,uint8_t *data_rd, size_t size);


/*
 *initialization i2c  mpu6050
 */
void Init_I2c_Master(void);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif 





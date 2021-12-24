/* i2c - Example

   For other examples please check:
   https://github.com/espressif/esp-idf/tree/master/examples

   See README.md file to get detailed usage of this example.

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include "esp_log.h"
#include "driver/i2c.h"
#include "sdkconfig.h"

#include "mpu6050_i2c.h"



/*
 * 写入一字节数据
 */
esp_err_t __attribute__((unused)) Mpu6050_i2c_master_write_Byte(i2c_port_t i2c_num,uint8_t adder, uint8_t data_wr)
{
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (MPU6060_SENSOR_ADDR << 1) | WRITE_BIT, ACK_CHECK_EN);   /*先写入器件地址*/
    i2c_master_write_byte(cmd, adder, ACK_CHECK_EN);   /*写入寄存器地址*/
    i2c_master_write_byte(cmd, data_wr, ACK_CHECK_EN);   /*写入数据*/    
    // i2c_master_write(cmd, data_wr, size, ACK_CHECK_EN);
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(i2c_num, cmd, 1000/portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);
    return ret;
}


/*
 * 读取一字节数据
 */
esp_err_t __attribute__((unused)) Mpu6050_i2c_master_read_Byte(i2c_port_t i2c_num, uint8_t adder,uint8_t *data_rd)
{
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (MPU6060_SENSOR_ADDR << 1) | WRITE_BIT, ACK_CHECK_EN);  /*发送器件地址+写命令	*/
    i2c_master_write_byte(cmd, adder, ACK_CHECK_EN);   /*写入寄存器地址*/
 
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (MPU6060_SENSOR_ADDR << 1) | READ_BIT, ACK_CHECK_EN);  /*发送器件地址+读命令	*/
    i2c_master_read_byte(cmd, data_rd, ACK_CHECK_EN);   /*读取1字节*/    
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(i2c_num, cmd, 1000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);
    return ret;
}


/*
 * 写入多字节节数据
 */
esp_err_t __attribute__((unused)) Mpu6050_i2c_master_write_slave(i2c_port_t i2c_num,uint8_t adder,uint8_t *data_wr,size_t size)
{
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (MPU6060_SENSOR_ADDR << 1) | WRITE_BIT, ACK_CHECK_EN);  /*先写入器件地址*/
    i2c_master_write_byte(cmd, adder, ACK_CHECK_EN);   /*写入寄存器地址*/
    i2c_master_write(cmd, data_wr, size, ACK_CHECK_EN);  /*写入数据*/
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(i2c_num, cmd, 1000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);
    return ret;
}


/*
 * 读取多字节数据
 */
esp_err_t __attribute__((unused)) Mpu6050_i2c_master_read_slave(i2c_port_t i2c_num, uint8_t adder,uint8_t *data_rd, size_t size)
{
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (MPU6060_SENSOR_ADDR << 1) | WRITE_BIT, ACK_CHECK_EN);  /*发送器件地址+写命令	*/
    i2c_master_write_byte(cmd, adder, ACK_CHECK_EN);   /*写入寄存器地址*/

    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (MPU6060_SENSOR_ADDR << 1) | READ_BIT, ACK_CHECK_EN);  /*发送器件地址+读命令	*/
    if(size > 1) {
        i2c_master_read(cmd, data_rd, size - 1, ACK_VAL);   /*读取数据*/
    }
    i2c_master_read_byte(cmd, data_rd + size - 1, NACK_VAL);  /*读取最后一个数据*/
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(i2c_num, cmd, 1000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);
    return ret;
}



/**
 * @brief i2c master initialization
 */
static esp_err_t i2c_master_init(void)
{
    int i2c_master_port = I2C_MASTER_NUM;
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = I2C_MASTER_FREQ_HZ,
        .clk_flags = I2C_SCLK_SRC_FLAG_FOR_NOMAL          /*!< Optional, you can use I2C_SCLK_SRC_FLAG_* flags to choose i2c source clock here. */
    };
    esp_err_t err = i2c_param_config(i2c_master_port, &conf);
    if (err != ESP_OK) {
        return err;
    }
    return i2c_driver_install(i2c_master_port, conf.mode, I2C_MASTER_RX_BUF_DISABLE, I2C_MASTER_TX_BUF_DISABLE, 0);
}

#if defined(CONFIG_IDF_TARGET_ESP32C3)
/**
 * @brief i2c slave initialization
 */
static esp_err_t i2c_slave_init(void)
{
    int i2c_slave_port = I2C_SLAVE_NUM;
    i2c_config_t conf_slave = {
        .sda_io_num = I2C_SLAVE_SDA_IO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_io_num = I2C_SLAVE_SCL_IO,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .mode = I2C_MODE_SLAVE,
        .slave.addr_10bit_en = 0,
        .slave.slave_addr = ESP_SLAVE_ADDR,
    };
    esp_err_t err = i2c_param_config(i2c_slave_port, &conf_slave);
    if (err != ESP_OK) {
        return err;
    }
    return i2c_driver_install(i2c_slave_port, conf_slave.mode, I2C_SLAVE_RX_BUF_LEN, I2C_SLAVE_TX_BUF_LEN, 0);
}

/**
 * @brief test function to show buffer
 */
static void disp_buf(uint8_t *buf, int len)
{
    int i;
    for (i = 0; i < len; i++) {
        printf("%02x ", buf[i]);
        if ((i + 1) % 16 == 0) {
            printf("\n");
        }
    }
    printf("\n");
}
#endif




/*
 *initialization i2c  mpu6050
 */
void Init_I2c_Master(void)
{
//   ESP_ERROR_CHECK(i2c_slave_init());
  ESP_ERROR_CHECK(i2c_master_init());   /*主机模式*/
}






#include <iostream>
#include <stdio.h>
#include <esp_log.h>



#include "mpu6050_i2c.h"
#include "mpu6050.h"
#include "inv_mpu.h"

static const char *TAG ="mpu6050";


Mpu6050::Mpu6050(/* args */)
{
 }

Mpu6050::~Mpu6050()
{
 }

/*********************************************************
*函数名:uint8_t MPU_Read_Byte(uint8_t reg) 	
*功能：IIC读一个字节 
*参数：addr:器件地址
/返回值:读到的数据
*********************************************************/
uint8_t Mpu6050::MPU_Read_Byte(uint8_t reg)
{
  Mpu6050_i2c_master_read_Byte(I2C_MASTER_NUM,reg,&data);
  return data;		
}

/*********************************************************
*函数名:uint8_t MPU_Write_Byte(uint8_t reg,uint8_t data) 	
*功能：IIC写一个字节 
*参数：addr:器件地址  data:数据
*返回值:返回值:0,成功
*       其他,错误代码
*********************************************************/
uint8_t Mpu6050::MPU_Write_Byte(uint8_t reg,uint8_t data) 				 
{ 
  return Mpu6050_i2c_master_write_Byte(I2C_MASTER_NUM,reg,data);
}


/*********************************************************
*函数名: uint8_t MPU_Set_Gyro_Fsr(uint8_t fsr)
*功能：设置MPU6050陀螺仪传感器满量程范围
*参数：fsr:0,±250dps;1,±500dps;2,±1000dps;3,±2000dps
*返回值:0,成功 1,失败
* 其他,错误代码
*********************************************************/
uint8_t Mpu6050::MPU_Set_Gyro_Fsr(uint8_t fsr)
{
	return MPU_Write_Byte(MPU_GYRO_CFG_REG,fsr<<3);//设置陀螺仪满量程范围  
}

/*********************************************************
*函数名: uint8_t MPU_Set_Accel_Fsr(uint8_t fsr)
*功能：设置MPU6050加速度传感器满量程范围
*参数：fsr:0,±2g;1,±4g;2,±8g;3,±16g
*返回值:0,成功 1,失败
* 其他,错误代码
*********************************************************/
uint8_t Mpu6050::MPU_Set_Accel_Fsr(uint8_t fsr)
{
	return MPU_Write_Byte(MPU_ACCEL_CFG_REG,fsr<<3);//设置加速度传感器满量程范围  
}


/*********************************************************
*函数名: uint8_t MPU_Set_LPF(uint16_t lpf)
*功能：设置MPU6050的数字低通滤波器
*参数：lpf:数字低通滤波频率(Hz)
*返回值:0,成功 1,失败
* 其他,错误代码
*********************************************************/
uint8_t Mpu6050::MPU_Set_LPF(uint16_t lpf)
{
	if(lpf>=188)data=1;
	else if(lpf>=98)data=2;
	else if(lpf>=42)data=3;
	else if(lpf>=20)data=4;
	else if(lpf>=10)data=5;
	else data=6; 
	return MPU_Write_Byte(MPU_CFG_REG,data);//设置数字低通滤波器  
}


/*********************************************************
*函数名: uint8_t MPU_Set_Rate(uint16_t rate)
*功能：设置MPU6050的采样率(假定Fs=1KHz)
*参数：rate:4~1000(Hz)
*返回值:0,成功 1,失败
* 其他,错误代码
*********************************************************/
uint8_t Mpu6050::MPU_Set_Rate(uint16_t rate)
{
	if(rate>1000)rate=1000;
	if(rate<4)rate=4;
	data=1000/rate-1;
	data=MPU_Write_Byte(MPU_SAMPLE_RATE_REG,data);	//设置数字低通滤波器
 	return MPU_Set_LPF(rate/2);	//自动设置LPF为采样率的一半
}


/*********************************************************
*函数名: uint8_t MPU_Set_Temperature_Fsr(uint8_t fsr)
*功能：设置MPU6050温度传感器开关
*参数：fsr:0开启  1关闭
*返回值:0,成功 1,失败
* 其他,错误代码
*********************************************************/
uint8_t Mpu6050::MPU_Set_Temperature_Fsr(uint8_t fsr)
{
	return MPU_Write_Byte(MPU_PWR_MGMT1_REG,fsr<<3);//设置陀螺仪满量程范围  
}

/*********************************************************
*函数名: short MPU_Get_Temperature(void)
*功能：得到温度值
*参数：rate:4~1000(Hz)
*返回值:温度值(扩大了100倍)
*********************************************************/
short Mpu6050::MPU_Get_Temperature(void)
{
	MPU_Read_Len(MPU_ADDR,MPU_TEMP_OUTH_REG,2,buf); 
    raw=((uint16_t)buf[0]<<8)|buf[1];  
    temp=36.53+((double)raw)/340;  
    return temp*100;;
}

/*********************************************************
*函数名: uint8_t MPU_Get_Gyroscope(short *gx,short *gy,short *gz)
*功能：得到陀螺仪值(原始值)
*参数：gx,gy,gz:陀螺仪x,y,z轴的原始读数(带符号)
*返回值:返回值:0,成功
*       其他,错误代码
*********************************************************/
uint8_t Mpu6050::MPU_Get_Gyroscope(short *gx,short *gy,short *gz)
{
 
	res=MPU_Read_Len(MPU_ADDR,MPU_GYRO_XOUTH_REG,6,buf);
	if(res==0)
	{
		*gx=((uint16_t)buf[0]<<8)|buf[1];  
		*gy=((uint16_t)buf[2]<<8)|buf[3];  
		*gz=((uint16_t)buf[4]<<8)|buf[5];
	} 	
    return res;;
}

/*********************************************************
*函数名: uint8_t MPU_Get_Accelerometer(short *ax,short *ay,short *az)
*功能：得到加速度值(原始值)
*参数：gx,gy,gz:陀螺仪x,y,z轴的原始读数(带符号)
*返回值:返回值:0,成功
*       其他,错误代码
*********************************************************/
uint8_t Mpu6050::MPU_Get_Accelerometer(short *ax,short *ay,short *az)
{

	res=MPU_Read_Len(MPU_ADDR,MPU_ACCEL_XOUTH_REG,6,buf);
	if(res==0)
	{
		*ax=((uint16_t)buf[0]<<8)|buf[1];  
		*ay=((uint16_t)buf[2]<<8)|buf[3];  
		*az=((uint16_t)buf[4]<<8)|buf[5];
	} 	
    return res;;
}

/*********************************************************
*函数名:void MPU6050_SLEEP_Iint(void)
*功能：mpu6050 进入低功耗模式
*参数：无
/返回值:无
*********************************************************/
void Mpu6050::Mpu6050_Sleep_Iint(void)
{
  MPU_Set_Temperature_Fsr(1);/*关闭温度传感器*/
  MPU_Write_Byte(MPU_PWR_MGMT2_REG,0X3F);	/*加速度与陀螺仪都工作 都处于休眠状态*/
}


/*********************************************************
*函数名:void Mpu6050_Wakepu_Iint(void)
*功能：mpu6050 推出休眠模式
*参数：无
/返回值:无
*********************************************************/
void Mpu6050::Mpu6050_Wakepu_Iint(void)
{
//	MPU_Write_Byte(MPU_PWR_MGMT2_REG,0X00);	
	MPU_Write_Byte(MPU_PWR_MGMT2_REG,0X07);	/*加速度与陀螺仪都工作 都处于休眠状态*/
}

/*********************************************************
*函数名: uint8_t MPU_Write_Len(uint8_t addr,uint8_t reg,uint8_t len,uint8_t *buf)
*功能：IIC连续写
*参数：addr:器件地址  reg:寄存器地址 len:写入长度 buf:数据区
*返回值:返回值:0,成功
*       其他,错误代码
*********************************************************/
uint8_t Mpu6050::MPU_Write_Len(uint8_t addr,uint8_t reg,uint8_t len,uint8_t *buf)
{
  return Mpu6050_i2c_master_write_slave(I2C_MASTER_NUM,reg,buf,len);
} 

/*********************************************************
*函数名:uint8_t MPU_Read_Len(uint8_t addr,uint8_t reg,uint8_t len,uint8_t *buf)
*功能：IIC连续读
*参数：addr:器件地址  reg:寄存器地址 len:写入长度 buf:数据区
*返回值:返回值:0,成功
*       其他,错误代码
*********************************************************/
uint8_t Mpu6050::MPU_Read_Len(uint8_t addr,uint8_t reg,uint8_t len,uint8_t *buf)
{ 
  return Mpu6050_i2c_master_read_slave(I2C_MASTER_NUM, reg,buf,len);
}



/*********************************************************
*函数名：uint8_t Mpu6050_Init(void)
*初始化MPU6050
*返回值:0,成功 
* 其他,错误代码
*********************************************************/
uint8_t Mpu6050::Mpu6050_Init(void)
{ 
 
  res=MPU_Read_Byte(MPU_DEVICE_ID_REG);   /*器件ID正确*/
  ESP_LOGI(TAG,"mpu6050 ID=0x%02X\r\n",res);
  if(res==MPU_ADDR){ 
//	MPU_Set_Gyro_Fsr(3);					/*陀螺仪传感器,±2000dps*/
//	MPU_Set_Accel_Fsr(0);					/*加速度传感器,±2g*/
//	MPU_Set_Rate(50);						/*设置采样率50Hz*/	 /*陀螺仪采样率，典型值：0x07(125Hz) */
     MPU_Write_Byte(MPU_INT_EN_REG,0X00);     	/*关闭所有中断*/
     MPU_Write_Byte(MPU_USER_CTRL_REG,0X00);	/*I2C主模式关闭*/
     MPU_Write_Byte(MPU_FIFO_EN_REG,0X00);	    /*关闭FIFO*/
     MPU_Write_Byte(MPU_INTBP_CFG_REG,0X80);	/*INT引脚低电平有效*/
	 MPU_Write_Byte(MPU_PWR_MGMT1_REG,0X01);	/*设置CLKSEL,PLL X轴为参考*/
	 MPU_Write_Byte(MPU_PWR_MGMT2_REG,0X00);	/*加速度与陀螺仪都工作*/
//	 MPU_Set_Rate(50);						/*设置采样率为50Hz*/
     MPU_Write_Byte(MPU_SAMPLE_RATE_REG, 0x07);    //陀螺仪采样率，典型值：0x07(125Hz)     
     MPU_Write_Byte(MPU_CFG_REG, 0x06);        //低通滤波频率，典型值：0x06(5Hz)     
     MPU_Write_Byte(MPU_GYRO_CFG_REG, 0x18);   //陀螺仪自检及测量范围，典型值：0x18(不自检，2000deg/s)     
     MPU_Write_Byte(MPU_ACCEL_CFG_REG, 0x01);  //加速计自检、测量范围及高通滤波频率，典型值：0x01(不自检，2G，5Hz) 	
//    MPU_Write_Byte(MPU6050_RA_FF_THR,0x01);             //自由落体阈值 
//    MPU_Write_Byte(0x20,0x01);             //自由落体检测时间20ms 单位1ms 寄存器0X20
//    MPU_Write_Byte(MPU6050_RA_ZRMOT_THR,0x20);          //静止阈值 
//    MPU_Write_Byte(0x20,0x20);          //静止检测时间20ms 单位1ms 寄存器0X20
//    MPU_Write_Byte(MPU_MOTION_DET_REG,0x01);     /*运动阈值 */
//    MPU_Write_Byte(0x20,0x14);                   /*检测时间20ms 单位1ms 寄存器0X20*/
//    MPU_Write_Byte(MPU_INT_EN_REG,0X40);	     /*中断使能寄存器*/
//	  MPU_Write_Byte(MPU_INTBP_CFG_REG,0X80);  	 /*INT引脚低电平有效*/
//    MPU_Write_Byte(MPU_CFG_REG,0x04);           //配置外部引脚采样和DLPF数字低通滤波器
//    MPU_Write_Byte(MPU6050_RA_ACCEL_CONFIG,0x1C);     //加速度传感器量程和高通滤波器配置
//    MPU_Write_Byte(MPU_INTBP_CFG_REG,0X1C);      //INT引脚低电平平时
//    MPU_Write_Byte(MPU6050_RA_INT_ENABLE,0x40);       //中断使能寄存器
      return 1;
 	  }
	return 0;
}

/*
 *initialization i2c  mpu6050
 */
void Mpu6050::init(void)
{

  Init_I2c_Master();   /*初始化I2C*/
  Mpu6050_Init();
}


/*
 *initialization i2c  mpu6050
 */
void Mpu6050::mpu_dmp_init(void)
{
	
  res=inv.mpu_dmp_init();
  ESP_LOGI(TAG,"mpu6050 ID=0x%02X\r\n",res);
}

uint8_t Mpu6050::mpu_dmp_get_data(float *pitch,float *roll,float *yaw)
{
 return inv.mpu_dmp_get_data(pitch,roll,yaw);
}
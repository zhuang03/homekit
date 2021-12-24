set(srcs "mpu6050.cpp" 
         "mpu6050_i2c.c" 
         "inv_mpu.cpp"
         "inv_mpu_dmp_motion_driver.cpp"
         )
 
idf_component_register(SRCS  ${srcs}
                       INCLUDE_DIRS "include"
					   REQUIRES lvgl)

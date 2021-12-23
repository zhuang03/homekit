set(mysrcs "lv_port_indev.c"
           "lv_port_fatfs.c"
           "lv_init.cpp"
           "lv_sd_fatfs.cpp"
           )
 
idf_component_register(SRCS  "${mysrcs}"
                       INCLUDE_DIRS "include"
					   REQUIRES lvgl fatfs lvgl_esp32_drivers)

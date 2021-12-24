set(srcs "app_main.cpp"
         "src/app_timer.cpp"
         "src/app_contorller.cpp"
         
         "app/homekit/homekit_app.cpp"
         "app/homekit/homekit_demo.c"
         "app/homekit/homekit_gpio.c"
         "app/homekit/homekit_rgb.c"
         "app/homekit/homekit_wifi.c"
       )

set(priv_includes  "src/include"
                   "app/homekit/include")


idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS ${priv_includes}
                       REQUIRES )

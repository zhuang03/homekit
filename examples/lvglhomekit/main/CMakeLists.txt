set(srcs "./app_main.cpp"
         "homekit/homekit_app.cpp"
         "homekit/homekit_demo.c"
         "homekit/homekit_gpio.c"
         "homekit/homekit_rgb.c"
         "homekit/homekit_wifi.c"

       )

set(priv_includes "homekit/include")


idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS ${priv_includes}
                      )

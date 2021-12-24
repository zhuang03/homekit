#include <stdio.h>
#include <string.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_log.h>

/*homekit*/
#include "homekit_app.h"
/*lvgl*/
#include "lv_init.h"
#include "lv_sd_fatfs.h"


Homekit hoit;
LvglInt lvgl;
SdCard sd; 


extern "C" void app_main()
{
  hoit.init();   
  hoit.wifi_connect(60*1000);
  sd.init();
  lvgl.init();
  lvgl.application();

  while (1)
  {
      /* code */
  }
   
}

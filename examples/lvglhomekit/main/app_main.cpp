#include <stdio.h>
#include <string.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_log.h>


#include "homekit_app.h"

Homekit hoit;

extern "C" void app_main()
{
 hoit.init();   
 hoit.wifi_connect(60*1000);

  while (1)
  {
      /* code */
  }
   
}

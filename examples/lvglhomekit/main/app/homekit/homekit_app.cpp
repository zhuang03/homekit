#include <iostream>
#include <thread>

#include "homekit_app.h"
#include "homekit_demo.h"
#include "homekit_gpio.h"
#include "homekit_rgb.h"
#include "homekit_wifi.h"

Homekit::Homekit(/* args */)
{
}

Homekit::~Homekit()
{
   
}



void Homekit::init(void)
{
   homekit_Bridge_init();
}


void Homekit::wifi_init(void)
{
   wifi_init();
}

void Homekit::wifi_connect(uint32_t delay)
{ 
   wifi_start_connect(delay);
}

void Homekit::smartconfig_connect(uint32_t delay)
{
   smartconfig_start_connect(delay);
}
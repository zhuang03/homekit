/*
 * ESPRESSIF MIT License
 *
 * Copyright (c) 2018 <ESPRESSIF SYSTEMS (SHANGHAI) PTE LTD>
 *
 * Permission is hereby granted for use on ESPRESSIF SYSTEMS products only, in which case,
 * it is free of charge, to any person obtaining a copy of this software and associated
 * documentation files (the "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished
 * to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

/* HomeKit Bridge Example*/

#include <stdio.h>
#include <string.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_log.h>

#include <hap_apple_servs.h>
#include <hap_apple_chars.h>

#include <iot_button.h>

#include <app_wifi.h>
#include <app_hap_setup_payload.h>




#include "homekit_demo.h"
#include "homekit_gpio.h"
#include "homekit_wifi.h"
#include "homekit_rgb.h"



/*
 *电灯   P19
 *摩天轮 P18
 *rgb
 *RED    P15
 *green  P2
 *biued  P4 
 */

static const char *TAG = "Bridge";

#define BRIDGE_TASK_PRIORITY    1
#define BRIDGE_TASK_STACKSIZE   4 * 1024
#define BRIDGE_TASK_NAME        "hap_bridge"


/* Reset network credentials if button is pressed for more than 3 seconds and then released */
#define RESET_NETWORK_BUTTON_TIMEOUT        3

/* Reset to factory if button is pressed and held for more than 10 seconds */
#define RESET_TO_FACTORY_BUTTON_TIMEOUT     10

/* The button "Boot" will be used as the Reset button for the example */
#define RESET_GPIO  GPIO_NUM_0



/**
 * @brief The network reset button callback handler.
 * Useful for testing the Wi-Fi re-configuration feature of WAC2
 */
static void reset_network_handler(void* arg)
{
    hap_reset_network();
}
/**
 * @brief The factory reset button callback handler.
 */
static void reset_to_factory_handler(void* arg)
{
    hap_reset_to_factory();
}


/**
 * The Reset button  GPIO initialisation function.
 * Same button will be used for resetting Wi-Fi network as well as for reset to factory based on
 * the time for which the button is pressed.
 */
static void reset_key_init(uint32_t key_gpio_pin)
{
    button_handle_t handle = iot_button_create(key_gpio_pin, BUTTON_ACTIVE_LOW);
    iot_button_add_on_release_cb(handle, RESET_NETWORK_BUTTON_TIMEOUT, reset_network_handler, NULL);
    iot_button_add_on_press_cb(handle, RESET_TO_FACTORY_BUTTON_TIMEOUT, reset_to_factory_handler, NULL);
}

/* Mandatory identify routine for the accessory (bridge)
 * In a real accessory, something like LED blink should be implemented
 * got visual identification
 */
static int bridge_identify(hap_acc_t *ha)
{
    ESP_LOGI(TAG, "Bridge identified");
    return HAP_SUCCESS;
}

/* Mandatory identify routine for the bridged accessory
 * In a real bridge, the actual accessory must be sent some request to
 * identify itself visually
 */
static int accessory_identify(hap_acc_t *ha)
{
    hap_serv_t *hs = hap_acc_get_serv_by_uuid(ha, HAP_SERV_UUID_ACCESSORY_INFORMATION);
    hap_char_t *hc = hap_serv_get_char_by_uuid(hs, HAP_CHAR_UUID_NAME);
    const hap_val_t *val = hap_char_get_val(hc);
    char *name = val->s;

    ESP_LOGI(TAG, "Bridged Accessory %s identified", name);
    return HAP_SUCCESS;
}



/* 
 * 摩天轮开关控制
 */
static int ferris_wheel_write(hap_write_data_t write_data[], int count,void *serv_priv, void *write_priv)
{

  ESP_LOGI(TAG, "Write ferris wheel for Accessory %s", (char *)serv_priv);
  int i, ret = HAP_SUCCESS;
  hap_write_data_t *write;
  for(i = 0; i < count; i++) {
      write = &write_data[i];
      if (!strcmp(hap_char_get_type_uuid(write->hc), HAP_CHAR_UUID_ON)) {
          ferris_wheel_on_off(write->val.b);    /*添加控制控制代码*/
          hap_char_update_val(write->hc, &(write->val));
          *(write->status) = HAP_STATUS_SUCCESS;
        }else {
          *(write->status) = HAP_STATUS_RES_ABSENT;
      }
    }
    return ret;
}


/* 
 *电灯控制
 */
static int electric_light_write(hap_write_data_t write_data[], int count,void *serv_priv, void *write_priv)
{

  ESP_LOGI(TAG, "Write electric light for Accessory %s", (char *)serv_priv);
  int i, ret = HAP_SUCCESS;
  hap_write_data_t *write;
  for (i = 0; i < count; i++) {
       write = &write_data[i];
       if(!strcmp(hap_char_get_type_uuid(write->hc), HAP_CHAR_UUID_ON)) {
          electric_light_on_off(write->val.b);    /*添加控制控制代码*/
          hap_char_update_val(write->hc, &(write->val));
          *(write->status) = HAP_STATUS_SUCCESS;
        } else {
          *(write->status) = HAP_STATUS_RES_ABSENT;
        }
    }
    return ret;
}


/* 
 *彩色灯控制
 */
static int lightbulb_write(hap_write_data_t write_data[], int count, void *serv_priv, void *write_priv)
{

 int i, ret = HAP_SUCCESS;
 hap_write_data_t *write;
 for(i = 0; i < count; i++){
     write = &write_data[i];
     /* Setting a default error value */
     *(write->status) = HAP_STATUS_VAL_INVALID;
     if(!strcmp(hap_char_get_type_uuid(write->hc), HAP_CHAR_UUID_ON)) {
        ESP_LOGI(TAG, "Received Write for Light %s", write->val.b ? "On" : "Off");
        if (lightbulb_set_on(write->val.b) == 0) {
            *(write->status) = HAP_STATUS_SUCCESS;
            }
        }else if(!strcmp(hap_char_get_type_uuid(write->hc), HAP_CHAR_UUID_BRIGHTNESS)) {
            ESP_LOGI(TAG, "Received Write for Light Brightness %d", write->val.i);
            if (lightbulb_set_brightness(write->val.i) == 0) {
                *(write->status) = HAP_STATUS_SUCCESS;
            }
        } else if(!strcmp(hap_char_get_type_uuid(write->hc), HAP_CHAR_UUID_HUE)) {
            ESP_LOGI(TAG, "Received Write for Light Hue %f", write->val.f);
            if (lightbulb_set_hue(write->val.f) == 0) {
                *(write->status) = HAP_STATUS_SUCCESS;
            }
        } else if(!strcmp(hap_char_get_type_uuid(write->hc), HAP_CHAR_UUID_SATURATION)) {
            ESP_LOGI(TAG, "Received Write for Light Saturation %f", write->val.f);
            if (lightbulb_set_saturation(write->val.f) == 0) {
                *(write->status) = HAP_STATUS_SUCCESS;
            }
        }else{
            *(write->status) = HAP_STATUS_RES_ABSENT;
        }

     /* If the characteristic write was successful, update it in hap core*/
     if (*(write->status) == HAP_STATUS_SUCCESS) {
         hap_char_update_val(write->hc, &(write->val));
        }else{
         /* Else, set the return value appropriately to report error */
        ret = HAP_FAIL;
        }
    }
 return ret;
}



/*The main thread for handling the Bridge Accessory */
void homekit_Bridge_init(void )
{
    hap_acc_t *accessory;
    hap_serv_t *service;

    /* Initialize the HAP core */
    hap_init(HAP_TRANSPORT_WIFI);

    /* Initialise the mandatory parameters for Accessory which will be added as
     * the mandatory services internally
     */
    hap_acc_cfg_t cfg = {
        .name = "Esp-Bridge",
        .manufacturer = "Espressif",
        .model = "EspBridge01",
        .serial_num = "001122334455",
        .fw_rev = "0.9.0",
        .hw_rev = NULL,
        .pv = "1.1.0",
        .identify_routine = bridge_identify,
        .cid = HAP_CID_BRIDGE,
    };
    /* Create accessory object */
    accessory = hap_acc_create(&cfg);

    /* Add a dummy Product Data */
    uint8_t product_data[] = {'E','S','P','3','2','H','A','P'};
    hap_acc_add_product_data(accessory, product_data, sizeof(product_data));

    /* Add the Accessory to the HomeKit Database */
    hap_add_accessory(accessory);
/*******************************************************************************************************************/
    /* Create and add the Accessory to the Bridge object*/

    char *accessory_name="开关";
    hap_acc_cfg_t bridge_cfg = {
            .name = accessory_name,
            .manufacturer = "Espressif",
            .model = "EspFan01",
            .serial_num = "abcdefg",
            .fw_rev = "0.9.0",
            .hw_rev = NULL,
            .pv = "1.1.0",
            .identify_routine = accessory_identify,
            .cid = HAP_CID_BRIDGE,
        };
    /* Create accessory object */
    accessory = hap_acc_create(&bridge_cfg);

    /* Create the Fan Service. Include the "name" since this is a user visible service  */
    service = hap_serv_fan_create(false);
    hap_serv_add_char(service, hap_char_name_create(accessory_name));
    // hap_serv_add_char(service, hap_char_rotation_direction_create(0));

    /* Set the Accessory name as the Private data for the service,
     * so that the correct accessory can be identified in the
     * write callback
     */
    hap_serv_set_priv(service, strdup(accessory_name));

    /* Set the write callback for the service */
    hap_serv_set_write_cb(service, ferris_wheel_write);
 
    /* Add the Fan Service to the Accessory Object */
    hap_acc_add_serv(accessory, service);

    /* Add the Accessory to the HomeKit Database */
    hap_add_bridged_accessory(accessory, hap_get_unique_aid(accessory_name));


    char *caideng_name="彩灯";
    hap_acc_cfg_t caideng_cfg = {
            .name = caideng_name,
            .manufacturer = "Espressif",
            .model = "EspFan01",
            .serial_num = "abcdefg",
            .fw_rev = "0.9.0",
            .hw_rev = NULL,
            .pv = "1.1.0",
            .identify_routine = accessory_identify,
            .cid = HAP_CID_BRIDGE,
        };
    /* Create accessory object */
    accessory = hap_acc_create(&caideng_cfg);

    /* Create the Fan Service. Include the "name" since this is a user visible service  */
    service = hap_serv_lightbulb_create(false);   /*创建服务*/
    hap_serv_add_char(service, hap_char_name_create(caideng_name));  /*名字*/
    hap_serv_add_char(service, hap_char_brightness_create(0));    /*默认亮度*/
    hap_serv_add_char(service, hap_char_hue_create(180));           /*默认颜色*/
    hap_serv_add_char(service, hap_char_saturation_create(0));     /*默认饱和读*/
 
    /* Set the Accessory name as the Private data for the service,
    * so that the correct accessory can be identified in the
    * write callback
    */
    hap_serv_set_priv(service, strdup(caideng_name));

    /* Set the write callback for the service */
    hap_serv_set_write_cb(service, lightbulb_write);
 
    /* Add the Fan Service to the Accessory Object */
    hap_acc_add_serv(accessory,service);

    /* Add the Accessory to the HomeKit Database */
    hap_add_bridged_accessory(accessory, hap_get_unique_aid(caideng_name));


   char *diandeng_name="电灯";
   hap_acc_cfg_t diandeng_cfg = {
            .name = diandeng_name,
            .manufacturer = "Espressif",
            .model = "EspFan01",
            .serial_num = "abcdefg",
            .fw_rev = "0.9.0",
            .hw_rev = NULL,
            .pv = "1.1.0",
            .identify_routine = accessory_identify,
            .cid = HAP_CID_BRIDGE,
        };
    /* Create accessory object */
    accessory = hap_acc_create(&diandeng_cfg);

    /* Create the Fan Service. Include the "name" since this is a user visible service  */
    service = hap_serv_lightbulb_create(false);   /*创建服务*/
    hap_serv_add_char(service, hap_char_name_create(diandeng_name));  /*名字*/


    /* Set the Accessory name as the Private data for the service,
    * so that the correct accessory can be identified in the
    * write callback
    */
    hap_serv_set_priv(service, strdup(diandeng_name));

    /* Set the write callback for the service */
    hap_serv_set_write_cb(service, electric_light_write);
 
    /* Add the Fan Service to the Accessory Object */
    hap_acc_add_serv(accessory,service);

    /* Add the Accessory to the HomeKit Database */
    hap_add_bridged_accessory(accessory, hap_get_unique_aid(diandeng_name));

    /************************************************************************/
    init_gpio();   
    lightbulb_init();   /*初始化引脚GPIO*/
    /* Register a common button for reset Wi-Fi network and reset to factory*/
    reset_key_init(RESET_GPIO);
    lightbulb_set_aim(0,0,0,0,0,0);
    /* For production accessories, the setup code shouldn't be programmed on to
     * the device. Instead, the setup info, derived from the setup code must
     * be used. Use the factory_nvs_gen utility to generate this data and then
     * flash it into the factory NVS partition.
     *
     * By default, the setup ID and setup info will be read from the factory_nvs
     * Flash partition and so, is not required to set here explicitly.
     *
     * However, for testing purpose, this can be overridden by using hap_set_setup_code()
     * and hap_set_setup_id() APIs, as has been done here.
     */
#ifdef CONFIG_EXAMPLE_USE_HARDCODED_SETUP_CODE
    /* Unique Setup code of the format xxx-xx-xxx. Default: 111-22-333 */
    hap_set_setup_code(CONFIG_EXAMPLE_SETUP_CODE);
    /* Unique four character Setup Id. Default: ES32 */
    hap_set_setup_id(CONFIG_EXAMPLE_SETUP_ID);
#ifdef CONFIG_APP_WIFI_USE_WAC_PROVISIONING
    app_hap_setup_payload(CONFIG_EXAMPLE_SETUP_CODE, CONFIG_EXAMPLE_SETUP_ID, true, cfg.cid);
#else
    app_hap_setup_payload(CONFIG_EXAMPLE_SETUP_CODE, CONFIG_EXAMPLE_SETUP_ID, false, cfg.cid);
#endif
#endif

    /* Enable Hardware MFi authentication (applicable only for MFi variant of SDK) */
    hap_enable_mfi_auth(HAP_MFI_AUTH_HW);

    /* Initialize Wi-Fi */
    // app_wifi_init();
    wifi_init();
    /* After all the initializations are done, start the HAP core */
    hap_start();
    /* Start Wi-Fi */
    // app_wifi_start(portMAX_DELAY);

    /* The task ends here. The read/write callbacks will be invoked by the HAP Framework */
    // vTaskDelete(NULL);
}





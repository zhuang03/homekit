/* WiFi station Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"

#include "lwip/err.h"
#include "lwip/sys.h"
#include "esp_smartconfig.h"    


#include "homekit_wifi.h"

/* The examples use WiFi configuration that you can set via project configuration menu
   If you'd rather not, just change the below entries to strings with
   the config you want - ie #define EXAMPLE_WIFI_SSID "mywifissid"
*/
static char* EXAMPLE_ESP_WIFI_SSID =  "Zexin";
static char* EXAMPLE_ESP_WIFI_PASS =  "9876543210";
#define EXAMPLE_ESP_MAXIMUM_RETRY   10

// #define WIFI_SEDN   

/* FreeRTOS event group to signal when we are connected*/
static EventGroupHandle_t s_wifi_event_group;

/* The event group allows multiple bits for each event, but we only care about two events:
 * - we are connected to the AP with an IP
 * - we failed to connect after the maximum amount of retries */

/* 宏定义WiFi连接事件标志位、连接失败标志位及智能配网标志位 */
#define WIFI_CONNECTED_BIT  BIT0
#define WIFI_FAIL_BIT       BIT1
#define SMART_CONFIG_BIT    BIT2

static const char *TAG = "wifi";
static int s_retry_num = 0;




/*保存wifi配置参数结构体变量wifi_config到nvs*/
static void saveWifiConfig(wifi_config_t *wifi_config)
{
  nvs_handle my_handle;
  nvs_open("WIFI_CONFIG", NVS_READWRITE, &my_handle); /*打开*/
  ESP_ERROR_CHECK(nvs_set_blob(my_handle, "wifi_config", wifi_config, sizeof(wifi_config_t)));
  ESP_ERROR_CHECK(nvs_commit(my_handle)); //提交
  nvs_close(my_handle);                   //退出
}


/*从nvs中读取wifi配置到给定的sta_config结构体变量*/
static esp_err_t readWifiConfig(wifi_config_t *sta_config)
{
  nvs_handle my_handle;
  nvs_open("WIFI_CONFIG", NVS_READWRITE, &my_handle); //打开
  unsigned int len = sizeof(wifi_config_t);
  esp_err_t err = nvs_get_blob(my_handle, "wifi_config", sta_config, &len);
  nvs_close(my_handle); //退出
  return err;
}


/* 系统事件循环处理函数 */
static void event_handler(void* arg, esp_event_base_t event_base,int32_t event_id, void* event_data)
{
    /* 系统事件为WiFi事件 */
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
            esp_wifi_connect();
            s_retry_num++;
            printf("retry to connect to the AP %d times. \n",s_retry_num);
            if (s_retry_num > 10)  /* WiFi重连次数大于10 */
            {
                /* 将WiFi连接事件标志组的WiFi连接失败事件位置1 */
                xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);
            }
        /* 清除WiFi连接成功标志位 */
         xEventGroupClearBits(s_wifi_event_group, WIFI_CONNECTED_BIT);

    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(TAG, "got ip:" IPSTR, IP2STR(&event->ip_info.ip));
        s_retry_num = 0;
        xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
    }
    else if (event_base == SC_EVENT)  /* 系统事件为智能配网事件 */
    {
        if(event_id == SC_EVENT_SCAN_DONE )             /* 开始扫描智能配网设备端 */
            printf("Scan done\n");
        else if(event_id == SC_EVENT_FOUND_CHANNEL)     /* 得到了智能配网通道 */
            printf("Found channel \n");
        else if(event_id == SC_EVENT_GOT_SSID_PSWD)     /* 得到了智能配网设备提供的ssid和password */
        {
            printf("smartconfig got SSID and password\n");
            /* 获取智能配网设备端提供的数据信息 */
            smartconfig_event_got_ssid_pswd_t *evt = (smartconfig_event_got_ssid_pswd_t *)event_data;
            /* 定义WiFi配置结构体和用了结收ssid和password的数组 */
            wifi_config_t wifi_config;
            uint8_t ssid[33] = { 0 };
            uint8_t password[65] = { 0 };

            bzero(&wifi_config, sizeof(wifi_config_t)); /* 将结构体数据清零 */
            /* 将智能配网设备发送来的WiFi的ssid、password及MAC地址复制到wifi_config */
            memcpy(wifi_config.sta.ssid, evt->ssid, sizeof(wifi_config.sta.ssid));
            memcpy(wifi_config.sta.password, evt->password, sizeof(wifi_config.sta.password));
            wifi_config.sta.bssid_set = evt->bssid_set;
            if (wifi_config.sta.bssid_set == true) {
                memcpy(wifi_config.sta.bssid, evt->bssid, sizeof(wifi_config.sta.bssid));
            }
            wifi_config.sta.threshold.authmode = WIFI_AUTH_WPA2_PSK;
            wifi_config.sta.pmf_cfg.capable = true;
            wifi_config.sta.pmf_cfg.required =false;
            saveWifiConfig(&wifi_config);  /*保存WIFI名称*/

            /* 打印WiFi名称和密码 */
            memcpy(ssid, evt->ssid, sizeof(evt->ssid));
            memcpy(password, evt->password, sizeof(evt->password));
            printf("SSID:%s \n", ssid);
            printf("PASSWORD:%s \n", password);

            /* 根据得到的WiFi名称和密码连接WiFi*/
            ESP_ERROR_CHECK( esp_wifi_disconnect() );
            ESP_ERROR_CHECK( esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config) );
            ESP_ERROR_CHECK( esp_wifi_connect() );
        }
        else if (event_id == SC_EVENT_SEND_ACK_DONE)    /* 智能配网成功，已经给智能配网设备发送应答*/
        {
            xEventGroupSetBits(s_wifi_event_group, SMART_CONFIG_BIT);
        }
    }
}

/*智能配网*/
void smartconfig_start_connect(uint32_t DelayTimer)
{

  ESP_ERROR_CHECK(esp_smartconfig_set_type(SC_TYPE_AIRKISS));  /* 设置智能配网类型为 AirKiss */
  smartconfig_start_config_t cfg = SMARTCONFIG_START_CONFIG_DEFAULT(); /* 通过SMARTCONFIG_START_CONFIG_DEFAULT宏 来获取一个默认的smartconfig配置参数结构体变量*/ 
  ESP_ERROR_CHECK( esp_smartconfig_start(&cfg) );  /* 开始智能配网 */
  printf("smartconfig start ....... \n");

  /* 使用事件标志组等待连接建立(WIFI_CONNECTED_BIT)或连接失败(WIFI_FAIL_BIT)事件 */
  EventBits_t uxBits;  /* 定义一个事件位变量来接收事件标志组等待函数的返回值 */
  uxBits = xEventGroupWaitBits(s_wifi_event_group, 
                               WIFI_CONNECTED_BIT | SMART_CONFIG_BIT, 
                               true, 
                               false, 
                               DelayTimer*1000);    /*portMAX_DELAY 等待事件标志组，退出前清除设置的事件标志，任意置1就会返回*/
  if(uxBits & WIFI_CONNECTED_BIT) {
     printf("WiFi Connected to ap ok. \n");
    }
  if(uxBits & SMART_CONFIG_BIT) {
     printf("smartconfig over \n");
     esp_smartconfig_stop(); /* 关闭智能配网 */
   }
}


/*连接网络*/
void wifi_start_connect(uint32_t DelayTimer)
{

  ESP_LOGI(TAG, "ESP_WIFI_MODE_STA");


  // Initialize default station as network interface instance (esp-netif) 
   /* 创建一个默认的WIFI-STA网络接口,如果初始化错误,此API将中止。*/
  esp_netif_t *sta_netif = esp_netif_create_default_wifi_sta();
  assert(sta_netif);

  wifi_config_t wifi_config;
  readWifiConfig(&wifi_config);  /*阅读WIFI的密钥*/
  char ssids[100]={0};
  memcpy(ssids, wifi_config.sta.ssid,sizeof(ssids));

  #ifdef WIFI_SEDN
  if(strlen(ssids)==0)
  #endif // DEBUG
  {
     ESP_LOGI(TAG, "WiFi is empty name"); 
     memcpy(wifi_config.sta.ssid, EXAMPLE_ESP_WIFI_SSID, sizeof(wifi_config.sta.ssid));
     memcpy(wifi_config.sta.password,EXAMPLE_ESP_WIFI_PASS, sizeof(wifi_config.sta.password));
     saveWifiConfig(&wifi_config);  /*保存WIFI名称*/
   }
  printf("SSID:%s \n", wifi_config.sta.ssid);
  printf("PASSWORD:%s \n", wifi_config.sta.password);


  wifi_config.sta.threshold.authmode = WIFI_AUTH_WPA2_PSK;
  wifi_config.sta.pmf_cfg.capable = true;
  wifi_config.sta.pmf_cfg.required =false;

    // wifi_config_t wifi_config = {
    //     .sta = {
    //         .ssid = "",
    //         .password = "",
    //         /* Setting a password implies station will connect to all security modes including WEP/WPA.
    //          * However these modes are deprecated and not advisable to be used. Incase your Access point
    //          * doesn't support WPA2, these mode can be enabled by commenting below line */
	//         .threshold.authmode = WIFI_AUTH_WPA2_PSK,
    //         .pmf_cfg = {
    //             .capable = true,
    //             .required = false
    //         },
    //     },
    // };

  ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA) ); /* 设置WiFi的工作模式为 STA */
  ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config) ); /* 设置WiFi连接的参数，主要是ssid和password */
  ESP_ERROR_CHECK(esp_wifi_start() );  /* 启动WiFi连接 */


  ESP_LOGI(TAG, "wifi_init_sta finished.");

  /* Waiting until either the connection is established (WIFI_CONNECTED_BIT) or connection failed for the maximum
   * number of re-tries (WIFI_FAIL_BIT). The bits are set by event_handler() (see above) */
  EventBits_t bits = xEventGroupWaitBits(s_wifi_event_group,/* 需要等待的事件标志组的句柄 */
                                         WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,/* 需要等待的事件位 */
                                         pdFALSE, /* 为pdFALSE时，在退出此函数之前所设置的这些事件位不变，为pdFALSE则清零*/ 
                                         pdFALSE,/* 为pdFALSE时，设置的这些事件位任意一个置1就会返回，为pdFALSE则需全为1才返回 */
                                         DelayTimer*1000); /* portMAX_DELAY 设置为最长阻塞等待时间，单位为时钟节拍 */

    /* xEventGroupWaitBits() returns the bits before the call returned, hence we can test which event actually * happened. */

  /*根据事件标志组等待函数的返回值获取WiFi连接状态 */
  if(bits & WIFI_CONNECTED_BIT) {
     ESP_LOGI(TAG, "connected to ap SSID:%s password:%s", EXAMPLE_ESP_WIFI_SSID, EXAMPLE_ESP_WIFI_PASS);
     vEventGroupDelete(s_wifi_event_group);                 /* 删除WiFi连接事件标志组，WiFi连接成功后不再需要 */
    }else if (bits & WIFI_FAIL_BIT) {
        ESP_LOGI(TAG, "Failed to connect to SSID:%s, password:%s",EXAMPLE_ESP_WIFI_SSID, EXAMPLE_ESP_WIFI_PASS);
        smartconfig_start_connect(60*1000);      /* 开启智能配网 */
    }else {
        ESP_LOGE(TAG, "UNEXPECTED EVENT");
        smartconfig_start_connect(60*1000);     /* 开启智能配网 */
    }

 /* The event will not be processed after unregister */
 // ESP_ERROR_CHECK(esp_event_handler_instance_unregister(IP_EVENT, IP_EVENT_STA_GOT_IP, instance_got_ip));
 // ESP_ERROR_CHECK(esp_event_handler_instance_unregister(WIFI_EVENT, ESP_EVENT_ANY_ID, instance_any_id));
 // vEventGroupDelete(s_wifi_event_group);
}




/*连接WIFI*/
void wifi_init(void)
{
  esp_err_t ret = nvs_flash_init();
  if(ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
     ESP_ERROR_CHECK(nvs_flash_erase()); /*删除保存的信息*/
     ret = nvs_flash_init();
    }
  ESP_ERROR_CHECK(ret);

  /*事件*/
  s_wifi_event_group = xEventGroupCreate();

  ESP_ERROR_CHECK(esp_netif_init()); /* 初始化底层TCP/IP堆栈。在应用程序启动时，应该调用此函数一次。*/
  ESP_ERROR_CHECK(esp_event_loop_create_default()); /* 创建默认事件循环*/

  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();  /*使用WIFI_INIT_CONFIG_DEFAULT()来获取一个默认的wifi配置参数结构体变量*/
  ESP_ERROR_CHECK(esp_wifi_init(&cfg));    /* 根据cfg参数初始化wifi连接所需要的资源 */


  /* 将事件处理程序注册到系统默认事件循环，分别是WiFi事件、IP地址事件及smartconfig事件 */
  esp_event_handler_instance_t instance_any_id;
  esp_event_handler_instance_t instance_got_ip;

  ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                      ESP_EVENT_ANY_ID,
                                                      &event_handler,
                                                      NULL,
                                                      &instance_any_id));
  ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
                                                      IP_EVENT_STA_GOT_IP,
                                                      &event_handler,
                                                      NULL,
                                                      &instance_got_ip));
    /*智能配网*/
  esp_event_handler_instance_t instance_sc_ip;
  ESP_ERROR_CHECK(esp_event_handler_instance_register(SC_EVENT,
                                                      ESP_EVENT_ANY_ID, 
                                                      &event_handler, 
                                                      NULL,
                                                      &instance_sc_ip));   /*智能配网*/

}






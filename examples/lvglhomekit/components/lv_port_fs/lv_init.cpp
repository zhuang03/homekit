/* LVGL Example project
 *
 * Basic project to test LVGL on ESP32 based projects.
 *
 * This example code is in the Public Domain (or CC0 licensed, at your option.)
 *
 * Unless required by applicable law or agreed to in writing, this
 * software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
 * CONDITIONS OF ANY KIND, either express or implied.
 */
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_freertos_hooks.h"
#include "freertos/semphr.h"
#include "esp_system.h"
#include "driver/gpio.h"
#include "esp_err.h"
#include "esp_log.h"
/* Littlevgl specific */
#ifdef LV_LVGL_H_INCLUDE_SIMPLE
#include "lvgl.h"
#else
#include "lvgl/lvgl.h"
#endif
#include "lvgl_helpers.h"



#include "lv_init.h"
#include "lv_port_fatfs.h"



/*
 *MOSI P27
 *CLK P26
 *CS P25
 *DC P32
 *REST P33
 */

/*********************
 *      DEFINES
 *********************/
#define TAG "lvgl"
#define LV_TICK_PERIOD_MS 1


#define BRIDGE_TASK_RUN_PRIORITY     2
#define BRIDGE_TASK_RUN_STACKSIZE    10 * 1024
#define BRIDGE_TASK_RUN_NAME        "gui"


void lv_tick_task(void *arg) {
(void) arg;

    lv_tick_inc(LV_TICK_PERIOD_MS);
}

lvglt::lvglt(/* args */)
{
}

lvglt::~lvglt()
{
}
 

void lvglt::init(void) 
{


    lv_init();

    /* Initialize SPI or I2C bus used by the drivers */
    lvgl_driver_init();
    
    lv_fs_if_init();   /*zxx*/


    lv_color_t* buf1 = (lv_color_t*)heap_caps_malloc(DISP_BUF_SIZE * sizeof(lv_color_t), MALLOC_CAP_DMA);
    assert(buf1 != NULL);

    /* Use double buffered when not working with monochrome displays */
#ifndef CONFIG_LV_TFT_DISPLAY_MONOCHROME
    lv_color_t* buf2 = (lv_color_t*)heap_caps_malloc(DISP_BUF_SIZE * sizeof(lv_color_t), MALLOC_CAP_DMA);
    assert(buf2 != NULL);
#else
    static lv_color_t *buf2 = NULL;
#endif

    static lv_disp_buf_t disp_buf;

    uint32_t size_in_px = DISP_BUF_SIZE;

#if defined CONFIG_LV_TFT_DISPLAY_CONTROLLER_IL3820         \
    || defined CONFIG_LV_TFT_DISPLAY_CONTROLLER_JD79653A    \
    || defined CONFIG_LV_TFT_DISPLAY_CONTROLLER_UC8151D     \
    || defined CONFIG_LV_TFT_DISPLAY_CONTROLLER_SSD1306

    /* Actual size in pixels, not bytes. */
    size_in_px *= 8;
#endif

    /* Initialize the working buffer depending on the selected display.
     * NOTE: buf2 == NULL when using monochrome displays. */
    lv_disp_buf_init(&disp_buf, buf1, buf2, size_in_px);

    lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.flush_cb = disp_driver_flush;

    /* When using a monochrome display we need to register the callbacks:
     * - rounder_cb
     * - set_px_cb */
#ifdef CONFIG_LV_TFT_DISPLAY_MONOCHROME
    disp_drv.rounder_cb = disp_driver_rounder;
    disp_drv.set_px_cb = disp_driver_set_px;
#endif

    disp_drv.buffer = &disp_buf;
    lv_disp_drv_register(&disp_drv);

    /* Register an input device when enabled on the menuconfig */
#if CONFIG_LV_TOUCH_CONTROLLER != TOUCH_CONTROLLER_NONE
    lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.read_cb = touch_driver_read;
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    lv_indev_drv_register(&indev_drv);
#endif

    /* Create and start a periodic timer interrupt to call lv_tick_inc */
    const esp_timer_create_args_t periodic_timer_args = {
        .callback = &lv_tick_task,
        .name = "periodic_gui"
     };
    esp_timer_handle_t periodic_timer;
    ESP_ERROR_CHECK(esp_timer_create(&periodic_timer_args, &periodic_timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(periodic_timer, LV_TICK_PERIOD_MS * 1000));

    // while (1) {
    //     /* Delay 1 tick (assumes FreeRTOS tick is 10ms */
    //     vTaskDelay(pdMS_TO_TICKS(10));
        
    //     /* Try to take the semaphore, call lvgl related function on success */
    //     if (pdTRUE == xSemaphoreTake(xGuiSemaphore, portMAX_DELAY)) {
    //         lv_task_handler();
    //         xSemaphoreGive(xGuiSemaphore);
    //       }
    // }

    /* A task should NEVER return */
    free(buf1);
#ifndef CONFIG_LV_TFT_DISPLAY_MONOCHROME
    free(buf2);
#endif
    // vTaskDelete(NULL);
}

void lvglt::application(void) 
{

//   lv_fs_file_t f;
//   lv_fs_dir_t d;
//   lv_fs_res_t res;
  
//   uint32_t resCount;
//   uint8_t buf[64];

//  lv_res_t we;
//  lv_img_decoder_dsc_t dsc;
//  we = lv_img_decoder_open(&dsc, "S:album/example_16bit.bmp", LV_COLOR_WHITE);
//  if(we == LV_RES_OK) {
//    /*Do something with `dsc->img_data`*/
//      lv_img_decoder_close(&dsc);
// }

// static lv_style_t style;
// lv_style_init(&style);
// lv_style_set_radius(&style, LV_STATE_DEFAULT, 5);
// /*Make a gradient*/
// lv_style_set_bg_opa(&style, LV_STATE_DEFAULT, LV_OPA_COVER);
// lv_style_set_bg_color(&style, LV_STATE_DEFAULT, LV_COLOR_RED);
// // lv_style_set_bg_grad_color(&style, LV_STATE_DEFAULT, LV_COLOR_RED);
// // lv_style_set_bg_grad_dir(&style, LV_STATE_DEFAULT, LV_GRAD_DIR_VER);
// /*Shift the gradient to the bottom*/
// lv_style_set_bg_main_stop(&style, LV_STATE_DEFAULT, 128);
// lv_style_set_bg_grad_stop(&style, LV_STATE_DEFAULT, 192);

// /*Create an object with the new style*/
// lv_obj_t * obj = lv_obj_create(lv_scr_act(), NULL);
// lv_obj_add_style(obj, LV_OBJ_PART_MAIN, &style);
// lv_obj_align(obj, NULL, LV_ALIGN_CENTER, 0, 0);


// 	//显示开机背景--windows
    lv_obj_t * objpg =  lv_img_create(lv_scr_act(), NULL);				
	lv_img_set_src(objpg, "S:weather/mao.bin");					
	lv_obj_align(objpg, NULL, LV_ALIGN_IN_TOP_LEFT, 2, 2);			
    lv_task_handler();

//   lv_obj_t * objpg =  lv_img_create(lv_scr_act(), NULL);				// 创建一个IMG对象 
//   lv_img_set_src(objpg, &dsc);					// 加载SD卡中的JPG图片
//   lv_obj_align(objpg, NULL, LV_ALIGN_IN_TOP_LEFT, 0, 0);			// 重新设置对齐

// lv_obj_test_start();
   /*用只读方式打开一个文件，来判断文件是否存在*/


	// res = lv_fs_open(&f,"S:hello.txt", LV_FS_MODE_RD);
	// if (res == LV_FS_RES_OK) {	// 文件存在
	// 	lv_fs_close(&f);		// 关闭文件
		ESP_LOGI(TAG,"'hello.txt' file is existsres : %d",12);
	// }else if(res == LV_FS_RES_UNKNOWN){// 文件不存在
	// 	ESP_LOGI(TAG,"'hello.txt' file is not exists res : %d",res);
	// }

 	// res = lv_fs_open(&f,"S:sdcard/hello.txt", LV_FS_MODE_WR);// 创建一个文件
	// if (res == LV_FS_RES_OK) {
	// 	ESP_LOGI(TAG,"Create file 'hello.txt' Success res : %d",res);
	// 	res = lv_fs_write(&f,"hellobug esp32", strlen("hellobug esp32"), &resCount);// 创建一个文件
	// 	if (res == LV_FS_RES_OK) {
	// 		ESP_LOGI(TAG,"Write file 'hello.txt' Success res : %d WriteByte: %d",res,resCount);
	// 	}		
	// }
	// lv_fs_close(&f);
 
	// //读取刚才创建的文件并打印内容
	// res = lv_fs_open(&f,"S:/hello.txt", LV_FS_MODE_RD);// 只读方式打开一个文件
	// if(res == LV_FS_RES_OK) {
	// 	ESP_LOGI(TAG,"lv_fs_open->Success %d",res);
	// 	memset(buf,0,64);
	// 	res = lv_fs_read(&f, buf, 64, &resCount);
	// 	if(res != LV_FS_RES_OK) {
	// 		ESP_LOGI(TAG,"lv_fs_read->Failed %d",res);
	// 	}else{
	// 		ESP_LOGI(TAG,"lv_fs_read->Success %d",res);
	// 	}
	// 	ESP_LOGI(TAG,"lv_fs_read ReadByte : %d Content : %s",resCount,buf);
	// 	lv_fs_close(&f);
	// }else{
	// 	ESP_LOGI(TAG,"lv_fs_open->Failed %d",res);
	// }
	// // 删除文件
	// res = lv_fs_remove("S:/bug.txt");
	// ESP_LOGI(TAG,"lv_fs_remove-> bug.txt   res:%d",res);
	// // 重命名文件
	// res = lv_fs_rename("S:/hello.txt","S:/bug.txt");
	// ESP_LOGI(TAG,"lv_fs_rename-> oldname:hello.txt     newname:bug.txt   res:%d",res);
	// //读取刚才改名的文件并打印内容
	// res = lv_fs_open(&f,"S:/bug.txt", LV_FS_MODE_RD);// 只读方式打开一个文件
	// if(res == LV_FS_RES_OK) {
	// 	// 获取文件大小
	// 	res = lv_fs_size(&f,&resCount);
	// 	ESP_LOGI(TAG,"lv_fs_size-> bug.txt   file size : %d Byte",resCount);
 
	// 	ESP_LOGI(TAG,"lv_fs_open-> S:/bug.txt Success %d",res);
	// 	memset(buf,0,64);
	// 	res = lv_fs_read(&f, buf, 64, &resCount);
	// 	if(res != LV_FS_RES_OK) {
	// 		ESP_LOGI(TAG,"lv_fs_read-> S:/bug.txt Failed %d",res);
	// 	}else{
	// 		ESP_LOGI(TAG,"lv_fs_read-> S:/bug.txt Success %d",res);
	// 	}
	// 	ESP_LOGI(TAG,"lv_fs_read S:/bug.txt ReadByte : %d Content : %s",resCount,buf);
 
	// 	// 定位文件操作指针示例
	// 	res = lv_fs_seek(&f, 7);
	// 	ESP_LOGI(TAG,"lv_fs_seek-> S:/bug.txt res %d",res);
	// 	// 获取文件操作指针示例
	// 	res = lv_fs_tell(&f, &resCount);
	// 	ESP_LOGI(TAG,"lv_fs_tell-> S:/bug.txt res %d   index: %d",res,resCount);
 
	// 	memset(buf,0,64);
	// 	res = lv_fs_read(&f, buf, 64, &resCount);
	// 	ESP_LOGI(TAG,"lv_fs_read seek 7Byte S:/bug.txt ReadByte : %d Content : %s",resCount,buf);
 
 
	// 	lv_fs_close(&f);
	// }else{
	// 	ESP_LOGI(TAG,"lv_fs_open-> S:/bug.txt Failed %d",res);
	// }
 
 
 
 
	// res = lv_fs_dir_open(&d,"S:/");
	// ESP_LOGI(TAG,"lv_fs_dir_open-> res %d",res);
 
	// while(1){
	// 	memset(buf,0,64);
	// 	res = lv_fs_dir_read(&d,(char *)buf);
	// 	if(buf[0] == '\0'){
	// 		break;
	// 	}
	// 	ESP_LOGI(TAG,"lv_fs_dir_read   res : %d Content : %s",res,buf);
	// }
	// res = lv_fs_dir_close(&d);
	// ESP_LOGI(TAG,"lv_fs_dir_close   res : %d ",res);

}




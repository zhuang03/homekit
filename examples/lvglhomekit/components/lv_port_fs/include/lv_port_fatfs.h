/**
 * @file lv_port_fs_templ.h
 *
 */

 /*Copy this file as "lv_port_fs.h" and set this value to "1" to enable content*/
#if 1

#ifndef _LV_PORT_FATFS_H_
#define _LV_PORT_FATFS_H_

#ifdef __cplusplus
extern "C" {
#endif

	/*********************
	 *      INCLUDES
	 *********************/
#include "lvgl.h"
#include "ff.h"
	 /*********************
	  *      DEFINES
	  *********************/
	void lv_fs_if_init(void);
	/**********************
	 *      TYPEDEFS
	 **********************/

	 /**********************
	  * GLOBAL PROTOTYPES
	  **********************/

	  /**********************
	   *      MACROS
	   **********************/


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_PORT_FS_TEMPL_H*/

#endif /*Disable/Enable content*/

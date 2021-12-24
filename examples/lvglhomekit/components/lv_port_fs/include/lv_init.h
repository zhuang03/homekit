#ifndef __LV_INIT_
#define __LV_INIT_

#ifdef __cplusplus
extern "C" {
#endif
 

#include <stdio.h>
#include <string.h>
/* Littlevgl specific */
#ifdef LV_LVGL_H_INCLUDE_SIMPLE
#include "lvgl.h"
#else
#include "lvgl/lvgl.h"
#endif
#include "lvgl_helpers.h"
  

class LvglInt
{
private:
    /* data */
public:
    void init(void);
    void application(void);

    LvglInt(/* args */);
    ~LvglInt();
};



#ifdef __cplusplus
} /* extern "C" */
#endif

#endif






































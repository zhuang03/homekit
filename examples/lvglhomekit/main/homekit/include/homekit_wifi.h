#ifndef _HOMEKIT_WIFI_H__
#define _HOMEKIT_WIFI_H__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>



#ifdef __cplusplus
extern "C" {
#endif
void wifi_init(void);


void wifi_start_connect(uint32_t DelayTimer);
/*智能配网*/
void smartconfig_start_connect(uint32_t DelayTimer);
#ifdef __cplusplus
}
#endif


#endif


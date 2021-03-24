#ifndef _LED_DRV_H_
#define _LED_DRV_H_

#include <stdlint.h>


/********************** New handler ***********************/
typedef enum {
   LED_STYLE_UNDEFINED = -1,
   LED_STYLE_NOT_CONFIGURED = 0,
   LED_STYLE_NORMAL = 1,
   LED_STYLE_SMARTCONFIG = 2,
   LED_STYLE_APCONFIG = 3,
   LED_STYLE_ON = 4,
   LED_STYLE_OFF = 5,
   LED_STYLE_UPDATE = 6
} led_stt_type_t;


#endif /* _LED_DRV_H_ */
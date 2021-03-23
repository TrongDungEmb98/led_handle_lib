
#ifndef _OS_APP_LED_H_
#define _OS_APP_LED_H_

#include <stdint.h>
#

typedef enum { LED_RUNNING = 1, LED_STOP = 2, LED_SUSPEND = 3 } state_object_led_t;

typedef enum {
    LED_TYPE_ONESHOT = 0,
    LED_TYPE_BLINK_NORMAL = 1,
    LED_TYPE_BLINK_FORERVER = 2,
    LED_TYPE_LED_ON = 3,
    LED_TYPE_LED_OFF = 4
} type_handle_led_t;

typedef struct {
    uint8_t led_pin;

    state_object_led_t current_object_state;
    type_handle_led_t type_handle;

    uint8_t state_turn_on_first_time;

    uint16_t count_blink;
    uint16_t on_interval_ms;
    uint16_t off_interval_ms;
    uint16_t current_interval_ms_on;
    uint16_t current_interval_ms_off;
    uint8_t current_state_on_off;
} os_led_control_t;

typedef void (*os_led_control_state_fn_t)(uint8_t pin, uint8_t state);
extern os_led_control_state_fn_t os_led_control_state;

void os_app_led_init(void);

os_led_control_t *insert_led_handle_oneshot(uint8_t pin, uint16_t interval_ms);

os_led_control_t *insert_led_handle_blink_normal(uint8_t pin, uint16_t interval_ms_on,
                                                 uint16_t interval_ms_off, uint16_t number_blink);

os_led_control_t *insert_led_handle_blink_forervel(uint8_t pin, uint16_t interval_ms_on,
                                                   uint16_t interval_ms_off);

void sync_function_handle_led(void *param);

void os_register_led_control(os_led_control_state_fn_t callback);

#endif  //_OS_APP_LED_H_


/*************** New handler ****************/


#include "app_led_driver.h"

#include "hardware.h"
#include "os_app_led.h"

#include "nrf_error.h"
#include "nrf.h"
#include "nrf_log.h"

#include "app_timer.h"
#include "nrf_drv_clock.h"

uint16_t led_stt_type = LED_STYLE_UNDEFINED;
uint16_t timeline_status[] = {200, 200, 200, 200, 200, 200, 200, 200};
uint8_t led_handle_count = 0;
uint8_t max_number_handle = 5;

void led_status_hanlder_callback(void *pArg)
{
    ret_code_t err_code;

    hw_toggle_led_status();
    led_handle_count = (led_handle_count + 1) % max_number_handle;

    /* Stop timer */
    err_code = app_timer_stop(app_timer_led);
    APP_ERROR_CHECK(err_code);
    /* start timer */
    app_timer_start(app_timer_led, APP_TIMER_TICKS(timeline_status[led_handle_count]), NULL);
}

void set_led_stt_smart_config(void)
{
    if (led_stt_type != LED_STYLE_NOT_CONFIGURED) {
        /* Stop timer */
        ret_code_t err_code = app_timer_stop(app_timer_led);
        APP_ERROR_CHECK(err_code);

        hw_led_stt_off();
        timeline_status[0] = 5000;
        timeline_status[1] = 200;
        led_handle_count = 0;
        max_number_handle = 2;
        led_stt_type = LED_STYLE_NOT_CONFIGURED;

        /* start task */
        app_timer_start(app_timer_led, APP_TIMER_TICKS(timeline_status[0]), NULL);
    }
}

void set_led_stt_ap_config()
{
   if (led_stt_type != LED_STYLE_APCONFIG)
   {
    /* Stop timer */
    ret_code_t err_code = app_timer_stop(app_timer_led);
    APP_ERROR_CHECK(err_code);

    hw_led_stt_off();

    timeline_status[0] = 500;
    timeline_status[1] = timeline_status[2] = timeline_status[3] = 200;
    led_handle_count = 0;
    max_number_handle = 4;
    led_stt_type = LED_STYLE_APCONFIG;

    /* start task */
    app_timer_start(app_timer_led, APP_TIMER_TICKS(timeline_status[0]), NULL);
   }
}

void app_led_status_init_and_start(void)
{
    /* GPIO Leds init */
    led_initialize();
    hw_toggle_led_status();

    /* init timer handler led */
    // err_code = app_timer_init();
    // APP_ERROR_CHECK(err_code);

    ret_code_t err_code  = app_timer_create(&app_timer_led,
                                            APP_TIMER_MODE_REPEATED,
                                            led_status_hanlder_callback);
    APP_ERROR_CHECK(err_code);

    /* start task */
    app_timer_start(app_timer_led, APP_TIMER_TICKS(timeline_status[0]), NULL);

    /* Test */
    set_led_stt_ap_config();
}
#include "hardware.h"
#include "os_app_led.h"

#include "nrf_error.h"
#include "nrf.h"
#include "nrf_log.h"

#include "app_timer.h"
#include "nrf_drv_clock.h"

#define TIME_MS_TASK_SYNC_LED 10  // ms
#define TIME_MS_INTERVAL_INDICATE_DEVICE_ACTIVE_ON 500
#define TIME_MS_INTERVAL_INDICATE_DEVICE_ACTIVE_OFF 500

#define APP_TIMER_LED_PERIOD            APP_TIMER_TICKS(TIME_MS_TASK_SYNC_LED) /* 1ms */

/* timer for handler led */
APP_TIMER_DEF(app_timer_led);
static os_led_control_t *pLedStatus = NULL;


void led_initialize(void)
{
    nrf_gpio_cfg_output(PIN_LED_STT);
    nrf_gpio_cfg_output(PIN_BUZZ);
    nrf_gpio_cfg_output(PIN_BUZZ_EXT);

    nrf_gpio_pin_write(PIN_LED_STT, 1);
    nrf_gpio_pin_write(PIN_BUZZ, 0);
    nrf_gpio_pin_write(PIN_BUZZ_EXT, 1);
}

void hw_control_led_on(void)
{
    nrf_gpio_pin_write(PIN_LED_STT, 0);
}

void hw_control_led_off(void)
{
    nrf_gpio_pin_write(PIN_LED_STT, 1);
}

void app_led_clear_status(void)
{
    if (pLedStatus != NULL) {
        pLedStatus->current_object_state = LED_STOP;
        pLedStatus = NULL;
    }

    hw_control_led_off();
}

/* Enable Pair and wait pair signal - Wait user config signal */
void app_set_led_device_active_state(void)
{
    app_led_clear_status();

    /* Led Yellow */
    if (pLedStatus == NULL) {
        pLedStatus = insert_led_handle_blink_forervel(
            PIN_LED_STT, TIME_MS_INTERVAL_INDICATE_DEVICE_ACTIVE_ON,
            TIME_MS_INTERVAL_INDICATE_DEVICE_ACTIVE_OFF);
    } else {
        pLedStatus->on_interval_ms = TIME_MS_INTERVAL_INDICATE_DEVICE_ACTIVE_ON;
        pLedStatus->off_interval_ms = TIME_MS_INTERVAL_INDICATE_DEVICE_ACTIVE_OFF;
        pLedStatus->current_interval_ms_on = TIME_MS_INTERVAL_INDICATE_DEVICE_ACTIVE_ON;
        pLedStatus->current_interval_ms_off = TIME_MS_INTERVAL_INDICATE_DEVICE_ACTIVE_OFF;

        pLedStatus->current_object_state = LED_RUNNING;
        pLedStatus->type_handle = LED_TYPE_BLINK_FORERVER;
    }
}

/* Enable Pair and wait pair signal - Wait user config signal */
void app_set_led_wait_pair_state(void)
{
    app_led_clear_status();

    /* Led Yellow */
    if (pLedStatus == NULL) {
        pLedStatus = insert_led_handle_blink_forervel(
            PIN_LED_STT, TIME_MS_INTERVAL_INDICATE_DEVICE_ACTIVE_ON,
            TIME_MS_INTERVAL_INDICATE_DEVICE_ACTIVE_OFF);
    } else {
        pLedStatus->on_interval_ms = TIME_MS_INTERVAL_INDICATE_DEVICE_ACTIVE_ON;
        pLedStatus->off_interval_ms = TIME_MS_INTERVAL_INDICATE_DEVICE_ACTIVE_OFF;
        pLedStatus->current_interval_ms_on = TIME_MS_INTERVAL_INDICATE_DEVICE_ACTIVE_ON;
        pLedStatus->current_interval_ms_off = TIME_MS_INTERVAL_INDICATE_DEVICE_ACTIVE_OFF;

        pLedStatus->current_object_state = LED_RUNNING;
        pLedStatus->type_handle = LED_TYPE_BLINK_FORERVER;
    }
}

static void app_led_control_state(uint8_t pin, uint8_t state)
{
    if (state == true) {
        hw_control_led_on();
    } else
        hw_control_led_off();
}

void app_led_handler_init_and_start(void)
{
    ret_code_t err_code;

    /* GPIO Leds init */
    led_initialize();

    /* porting led_lib */
    os_register_led_control(app_led_control_state);
    os_app_led_init();

    /* init timer handler led */
    err_code = app_timer_create(&app_timer_led,
                            APP_TIMER_MODE_REPEATED,
                            sync_function_handle_led);
    APP_ERROR_CHECK(err_code);

    /* start task */
    app_timer_start(app_timer_led, APP_TIMER_LED_PERIOD, NULL);

    /* Test */
    app_set_led_device_active_state();
}

int main()
{
	app_led_handler_init_and_start();

	while(1) {
		/* do something */
	}
	return 0;
}
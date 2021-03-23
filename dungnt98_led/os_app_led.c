#include "os_app_led.h"

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#define MAXIMUM_LED_CONTROL 16

#define TIME_MS_TASK_SYNC_LED 10  // ms

#define TURN_ON_FIRST_TIME 1
#define NOT_TURN_ON_FIRST_TIME 0

#define LED_STATE_IS_ON 1
#define LED_STATE_IS_OFF 0

static os_led_control_t m_os_led[MAXIMUM_LED_CONTROL];
static uint8_t count_led_handle = 0;

os_led_control_state_fn_t os_led_control_state;

void os_register_led_control(os_led_control_state_fn_t callback)
{
    os_led_control_state = callback;
}

os_led_control_t *insert_led_handle_oneshot(uint8_t pin, uint16_t interval_ms)
{
    for (uint8_t i = 0; i < MAXIMUM_LED_CONTROL; i++) {
        if (m_os_led[i].current_object_state != LED_RUNNING) {
            memset(&m_os_led[i], 0, sizeof(os_led_control_t));
            m_os_led[i].led_pin = pin;

            m_os_led[i].current_object_state = LED_RUNNING;
            m_os_led[i].type_handle = LED_TYPE_ONESHOT;
            m_os_led[i].state_turn_on_first_time = NOT_TURN_ON_FIRST_TIME;
            m_os_led[i].current_state_on_off = LED_STATE_IS_OFF;

            m_os_led[i].on_interval_ms = interval_ms;
            m_os_led[i].current_interval_ms_on = interval_ms;

            count_led_handle++;
            return &m_os_led[i];
        }
    }

    return NULL;
}

os_led_control_t *insert_led_handle_blink_normal(uint8_t pin, uint16_t interval_ms_on,
                                                 uint16_t interval_ms_off, uint16_t number_blink)
{
    for (uint8_t i = 0; i < MAXIMUM_LED_CONTROL; i++) {
        if (m_os_led[i].current_object_state != LED_RUNNING) {
            memset(&m_os_led[i], 0, sizeof(os_led_control_t));
            m_os_led[i].led_pin = pin;
            m_os_led[i].current_object_state = LED_RUNNING;
            m_os_led[i].type_handle = LED_TYPE_BLINK_NORMAL;
            m_os_led[i].state_turn_on_first_time = NOT_TURN_ON_FIRST_TIME;
            m_os_led[i].current_state_on_off = LED_STATE_IS_OFF;

            m_os_led[i].on_interval_ms = interval_ms_on;
            m_os_led[i].current_interval_ms_on = interval_ms_on;

            m_os_led[i].off_interval_ms = interval_ms_off;
            m_os_led[i].current_interval_ms_off = interval_ms_off;

            m_os_led[i].count_blink = number_blink;

            count_led_handle++;
            return &m_os_led[i];
        }
    }

    return NULL;
}

os_led_control_t *insert_led_handle_blink_forervel(uint8_t pin, uint16_t interval_ms_on,
                                                   uint16_t interval_ms_off)
{
    for (uint8_t i = 0; i < MAXIMUM_LED_CONTROL; i++) {
        if (m_os_led[i].current_object_state != LED_RUNNING) {
            memset(&m_os_led[i], 0, sizeof(os_led_control_t));
            m_os_led[i].led_pin = pin;
            m_os_led[i].current_object_state = LED_RUNNING;
            m_os_led[i].type_handle = LED_TYPE_BLINK_FORERVER;
            m_os_led[i].state_turn_on_first_time = NOT_TURN_ON_FIRST_TIME;
            m_os_led[i].current_state_on_off = LED_STATE_IS_OFF;

            m_os_led[i].on_interval_ms = interval_ms_on;
            m_os_led[i].current_interval_ms_on = interval_ms_on;

            m_os_led[i].off_interval_ms = interval_ms_off;
            m_os_led[i].current_interval_ms_off = interval_ms_off;

            count_led_handle++;
            return &m_os_led[i];
        }
    }

    return NULL;
}

void os_app_led_init(void)
{
    /* Reset buffer os led */
    memset(m_os_led, 0, sizeof(m_os_led));
    count_led_handle = 0;
}

void sync_function_handle_led(void *param)
{
    uint8_t current_led_handle = 0;
    if (count_led_handle <= 0)
        return;

    for (uint8_t i = 0; i < MAXIMUM_LED_CONTROL; i++) {
        if (current_led_handle == count_led_handle)
            return;

        if (m_os_led[i].current_object_state == LED_RUNNING) {
            current_led_handle++;
            if (m_os_led[i].state_turn_on_first_time == NOT_TURN_ON_FIRST_TIME) {
                /* Turn on led */
                os_led_control_state(m_os_led[i].led_pin, true);
                m_os_led[i].state_turn_on_first_time = TURN_ON_FIRST_TIME;
                m_os_led[i].current_state_on_off = LED_STATE_IS_ON;
                continue;
            }

            switch (m_os_led[i].type_handle) {
                case LED_TYPE_ONESHOT:
                    m_os_led[i].current_interval_ms_on -= TIME_MS_TASK_SYNC_LED;
                    if (m_os_led[i].current_interval_ms_on < TIME_MS_TASK_SYNC_LED) {
                        // turn off led
                        os_led_control_state(m_os_led[i].led_pin, false);
                        /* Set state led stop */
                        m_os_led[i].current_object_state = LED_STOP;
                        count_led_handle--;
                        current_led_handle--;
                    }

                    break;

                case LED_TYPE_BLINK_NORMAL:
                    if (m_os_led[i].current_state_on_off == LED_STATE_IS_ON) {
                        m_os_led[i].current_interval_ms_on -= TIME_MS_TASK_SYNC_LED;

                        if (m_os_led[i].current_interval_ms_on < TIME_MS_TASK_SYNC_LED) {
                            // Turn off led
                            os_led_control_state(m_os_led[i].led_pin, false);
                            m_os_led[i].count_blink--;
                            /* if is last count blink */
                            if (m_os_led[i].count_blink == 0) {
                                m_os_led[i].current_object_state = LED_STOP;

                                count_led_handle--;
                                current_led_handle--;

                            } else {
                                m_os_led[i].current_state_on_off = LED_STATE_IS_OFF;
                                m_os_led[i].current_interval_ms_on = m_os_led[i].on_interval_ms;
                            }
                        }
                    }

                    else {
                        m_os_led[i].current_interval_ms_off -= TIME_MS_TASK_SYNC_LED;
                        if (m_os_led[i].current_interval_ms_off < TIME_MS_TASK_SYNC_LED) {
                            // Turn On led
                            os_led_control_state(m_os_led[i].led_pin, true);
                            m_os_led[i].current_state_on_off = LED_STATE_IS_ON;
                            m_os_led[i].current_interval_ms_off = m_os_led[i].off_interval_ms;
                        }
                    }

                    break;

                case LED_TYPE_BLINK_FORERVER:

                    if (m_os_led[i].current_state_on_off == LED_STATE_IS_ON) {
                        m_os_led[i].current_interval_ms_on -= TIME_MS_TASK_SYNC_LED;

                        if (m_os_led[i].current_interval_ms_on < TIME_MS_TASK_SYNC_LED) {
                            // Turn off led
                            os_led_control_state(m_os_led[i].led_pin, false);

                            m_os_led[i].current_state_on_off = LED_STATE_IS_OFF;
                            m_os_led[i].current_interval_ms_on = m_os_led[i].on_interval_ms;
                        }
                    }

                    else {
                        m_os_led[i].current_interval_ms_off -= TIME_MS_TASK_SYNC_LED;
                        if (m_os_led[i].current_interval_ms_off < TIME_MS_TASK_SYNC_LED) {
                            // Turn On led
                            os_led_control_state(m_os_led[i].led_pin, true);
                            m_os_led[i].current_state_on_off = LED_STATE_IS_ON;
                            m_os_led[i].current_interval_ms_off = m_os_led[i].off_interval_ms;
                        }
                    }

                    break;

                default:

                    break;
            }
        }
    }
}

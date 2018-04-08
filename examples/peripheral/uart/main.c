
#include "nrf.h"
#include "nrf_gpio.h"
#include "nrfx_clock.h"
#include "app_timer.h"
#include "nrf_delay.h"
#include "button.h"

#include "LINDrivers/lin_app.h"

// Hardware definition

#define IO0 1
#define IO1 3
#define IO2 5
#define IO3 7
#define IO4 6
#define IO5 4
#define IO6 2
#define IO7 0

#define IGNITION_STARTER_POS_1_PIN IO0
#define IGNITION_STARTER_POS_2_PIN IO1
#define WINDSHIELD_WIPERS_SLOW_PIN IO2
#define WINDSHIELD_WIPERS_FAST_PIN IO3
#define TURN_SIGNAL_LEFT_PIN  IO4
#define TURN_SIGNAL_RIGHT_PIN IO5
#define HEADLIGHT_DIMMER_PIN IO6
#define HORN_BUTTON_PIN IO7

#define MCP2050_CS_LWAKE_PIN 29

#define BLINKER_TOGGLE_MS (90*1000/60/2) // 90 flashes per minutes as specified by SAE J590b
APP_TIMER_DEF(blinker_timer_id);
static void blinker_timer(void* p_context) {
    BLINKER_Data[0] ^= 1;
}

static void clock_event_handler(nrfx_clock_evt_type_t event)
{
}

#define DEBOUNCE_TICKS APP_TIMER_TICKS(20) // 20 ms debouncing

static bool engine_started = false;

static button_t ignition_starter_pos_1;
static button_t ignition_starter_pos_2;
static button_t windshield_wipers_slow;
static button_t windshield_wipers_fast;
static button_t turn_signal_left;
static button_t turn_signal_right;
static button_t headlight_dimmer;
static button_t horn_button;

static void turn_signal_left_pressed(void) {
    if (button_is_released(ignition_starter_pos_1))
        return;
    TURN_SIGNAL_Data[0] |= 1;
}

static void turn_signal_left_released(void) {
    TURN_SIGNAL_Data[0] &= ~1;
}

static void turn_signal_right_pressed(void) {
    if (button_is_released(ignition_starter_pos_1))
        return;
    TURN_SIGNAL_Data[0] |= 2;
}

static void turn_signal_right_released(void) {
    TURN_SIGNAL_Data[0] &= ~2;
}

static void wipers_changed(void) {
    if (button_is_released(ignition_starter_pos_1))
        return;
    if (button_is_pressed(windshield_wipers_fast)) {
        WIPERS_Data[0] = 2;
    } else if (button_is_pressed(windshield_wipers_slow)) {
        WIPERS_Data[0] = 1;
    } else {
        WIPERS_Data[0] = 0;
    }
}

static void ignition_starter_pos_1_pressed(void) {
    IGNITION_STARTER_Data[0] |= 1;
}

static void ignition_starter_pos_1_released(void) {
    engine_started = false;
    IGNITION_STARTER_Data[0] &= ~1;
    LIGHTS_Data[0] = 0;
    WIPERS_Data[0] = 0;
    TURN_SIGNAL_Data[0] = 0;
}

static void ignition_starter_pos_2_pressed(void) {
    IGNITION_STARTER_Data[0] |= 2;
    engine_started = true;
}

static void ignition_starter_pos_2_released(void) {
    IGNITION_STARTER_Data[0] &= ~2;
    if (engine_started && (LIGHTS_Data[0] == 0)) {
        // We turn the low beam lights ON as soon as we're done cranking the engine
        LIGHTS_Data[0] = 1;
    }
}

static void headlight_dimmer_pressed(void) {
    if (engine_started) {
        LIGHTS_Data[0] ^= 3;
    }
}

static void horn_button_pressed(void) { HORN_Data[0] = 1; }
static void horn_button_released(void) { HORN_Data[0] = 0; }

/**
 * @brief Function for main application entry.
 */
int main(void)
{
    // Wait 5 seconds to give a chance to debug
    nrf_delay_ms(5000);

    APP_ERROR_CHECK(nrfx_clock_init(clock_event_handler));
    nrfx_clock_enable();
    nrfx_clock_lfclk_start();

    // Configure GPIO
    ignition_starter_pos_1 = button_init(IGNITION_STARTER_POS_1_PIN, DEBOUNCE_TICKS, ignition_starter_pos_1_pressed, ignition_starter_pos_1_released);
    ignition_starter_pos_2 = button_init(IGNITION_STARTER_POS_2_PIN, DEBOUNCE_TICKS, ignition_starter_pos_2_pressed, ignition_starter_pos_2_released);
    windshield_wipers_slow = button_init(WINDSHIELD_WIPERS_SLOW_PIN, DEBOUNCE_TICKS, wipers_changed, wipers_changed);
    windshield_wipers_fast = button_init(WINDSHIELD_WIPERS_FAST_PIN, DEBOUNCE_TICKS, wipers_changed, wipers_changed);
    turn_signal_left = button_init(TURN_SIGNAL_LEFT_PIN, DEBOUNCE_TICKS, turn_signal_left_pressed, turn_signal_left_released);
    turn_signal_right = button_init(TURN_SIGNAL_RIGHT_PIN, DEBOUNCE_TICKS, turn_signal_right_pressed, turn_signal_right_released);
    headlight_dimmer = button_init(HEADLIGHT_DIMMER_PIN, DEBOUNCE_TICKS, headlight_dimmer_pressed, NULL);
    horn_button = button_init(HORN_BUTTON_PIN, DEBOUNCE_TICKS, horn_button_pressed, horn_button_released);

    // Wake-up the MCP2050
    nrf_gpio_cfg_output(MCP2050_CS_LWAKE_PIN);
    nrf_gpio_pin_write(MCP2050_CS_LWAKE_PIN, 1);

    // Initialize the timer subsystem
    APP_ERROR_CHECK(app_timer_init());

    lin_app_initialize();

    APP_ERROR_CHECK(app_timer_create(
               &blinker_timer_id,
               APP_TIMER_MODE_REPEATED,
               blinker_timer));
    APP_ERROR_CHECK(app_timer_start(
               blinker_timer_id,
               APP_TIMER_TICKS(BLINKER_TOGGLE_MS),
               NULL));

    while (true)
    {
        button_execute();
    }
}

/** @} */

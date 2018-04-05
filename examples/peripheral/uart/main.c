
#include "nrf.h"
//#include "bsp.h"
#include "nrf_gpio.h"
#include "nrfx_clock.h"

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

/**
 * @brief Function for main application entry.
 */
int main(void)
{
    nrfx_clock_lfclk_start();

    // Configure GPIO
    nrf_gpio_cfg_input(IGNITION_STARTER_POS_1_PIN, NRF_GPIO_PIN_NOPULL);
    nrf_gpio_cfg_input(IGNITION_STARTER_POS_2_PIN, NRF_GPIO_PIN_NOPULL);
    nrf_gpio_cfg_input(WINDSHIELD_WIPERS_SLOW_PIN, NRF_GPIO_PIN_NOPULL);
    nrf_gpio_cfg_input(WINDSHIELD_WIPERS_FAST_PIN, NRF_GPIO_PIN_NOPULL);
    nrf_gpio_cfg_input(TURN_SIGNAL_LEFT_PIN, NRF_GPIO_PIN_NOPULL);
    nrf_gpio_cfg_input(TURN_SIGNAL_RIGHT_PIN, NRF_GPIO_PIN_NOPULL);
    nrf_gpio_cfg_input(HEADLIGHT_DIMMER_PIN, NRF_GPIO_PIN_NOPULL);
    nrf_gpio_cfg_input(HORN_BUTTON_PIN, NRF_GPIO_PIN_NOPULL);

    // Wake-up the MCP2050
    nrf_gpio_cfg_output(MCP2050_CS_LWAKE_PIN);
    nrf_gpio_pin_write(MCP2050_CS_LWAKE_PIN, 1);

    lin_app_initialize();

    while (true)
    {
        static bool engine_started = false;
        if (!nrf_gpio_pin_read(IGNITION_STARTER_POS_1_PIN)) {
            IGNITION_STARTER_Data[0] |= 1;
        } else {
            engine_started = false;
            IGNITION_STARTER_Data[0] &= ~1;
            LIGHTS_Data[0] = 0;
            WIPERS_Data[0] = 0;
            BLINKER_Data[0] = 0;
        }

        if (!nrf_gpio_pin_read(IGNITION_STARTER_POS_2_PIN)) {
            IGNITION_STARTER_Data[0] |= 2;
            engine_started = true;
        } else {
            IGNITION_STARTER_Data[0] &= ~1;
            // We turn the low beam lights ON as soon as we're done cranking the engine
            LIGHTS_Data[0] = 1;
        }

        if (!nrf_gpio_pin_read(WINDSHIELD_WIPERS_FAST_PIN)) {
            WIPERS_Data[0] = 2;
        } else if (!nrf_gpio_pin_read(WINDSHIELD_WIPERS_SLOW_PIN)) {
            WIPERS_Data[0] = 1;
        } else {
            WIPERS_Data[0] = 0;
        }

        if (!nrf_gpio_pin_read(TURN_SIGNAL_LEFT_PIN)) {
            BLINKER_Data[0] |= 1;
        } else {
            BLINKER_Data[0] &= ~1;
        }

        if (!nrf_gpio_pin_read(TURN_SIGNAL_RIGHT_PIN)) {
            BLINKER_Data[0] |= 2;
        } else {
            BLINKER_Data[0] &= ~2;
        }

        static uint32_t headlight_dimmer_previous_state = 1;
        uint32_t headlight_dimmer_new_state = nrf_gpio_pin_read(HEADLIGHT_DIMMER_PIN);
        if (engine_started && headlight_dimmer_previous_state && !headlight_dimmer_new_state) {
            LIGHTS_Data[0] ^= 3;
        }
        headlight_dimmer_previous_state = headlight_dimmer_new_state;

        if (!nrf_gpio_pin_read(HORN_BUTTON_PIN)) {
            HORN_Data[0] = 1;
        } else {
            HORN_Data[0] = 0;
        }

        // TODO update blinker
    }
}

/** @} */

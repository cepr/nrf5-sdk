
#include "button.h"
#include "nrf.h"
#include "app_timer.h"
#include "nrf_gpio.h"

struct button_s {
    uint32_t pin;
    uint32_t debounce_ticks;
    uint32_t last_change;
    uint32_t value;
    void (*on_press)(void);
    void (*on_release)(void);
};

#define MAX_BUTTON 10
static struct button_s buttons[MAX_BUTTON];
static int button_count = 0;

button_t button_init(uint32_t pin_number, uint32_t debounce_ticks, void (*on_press)(void), void (*on_release)(void))
{
    APP_ERROR_CHECK_BOOL(button_count < MAX_BUTTON);

    // Configure the pin
    nrf_gpio_cfg_input(pin_number, NRF_GPIO_PIN_NOPULL);

    // Allocate and configure a button
    button_t ret = &buttons[button_count++];
    ret->pin = pin_number;
    ret->debounce_ticks = debounce_ticks;
    ret->last_change = app_timer_cnt_get();
    ret->value = nrf_gpio_pin_read(pin_number);
    ret->on_press = on_press;
    ret->on_release = on_release;
    return ret;
}

void button_execute(void)
{
    int i;
    for (i = 0; i < button_count; i++) {
        button_t b = &buttons[i];

        uint32_t now = app_timer_cnt_get();
        if (app_timer_cnt_diff_compute(now, b->last_change) < b->debounce_ticks) {
            // ignore any pin change as long as the debouncing period is not over
            continue;
        }

        // read new GPIO value and compare
        uint32_t gpio_value = nrf_gpio_pin_read(b->pin);
        if (gpio_value == b->value) {
            // no change
            continue;
        }
        // pin value has changed since last call to this function

        b->value = gpio_value;
        b->last_change = now;

        if (b->on_press && !gpio_value) {
            // Check for button press
            b->on_press();
        }

        if (b->on_release && gpio_value) {
            // Check for button release
            b->on_release();
        }

    }
}

bool button_is_pressed(button_t button)
{
    return button->value == 0;
}

bool button_is_released(button_t button)
{
    return button->value != 0;
}

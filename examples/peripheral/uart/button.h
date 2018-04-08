#ifndef _BUTTON_H_
#define _BUTTON_H_

#include <stdint.h>
#include <stdbool.h>

typedef struct button_s *button_t;

button_t button_init(uint32_t pin_number, uint32_t debounce_ticks, void (*on_press)(void), void (*on_release)(void));
void button_execute(void);
bool button_is_pressed(button_t button);
bool button_is_released(button_t button);

#endif

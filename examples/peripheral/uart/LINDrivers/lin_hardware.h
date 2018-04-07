#ifndef LIN_HARDWARE_H
#define LIN_HARDWARE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

void lin_hardware_uart_init(uint32_t baudrate);
void lin_hardware_uart_set_baudrate(uint32_t baudrate);
void lin_hardware_uart_write(const uint8_t *data, size_t length, void (*tx_complete)(void));
void lin_hardware_uart_enable_rx(void (*on_receive_callback)(uint8_t));
void lin_hardware_uart_disable_rx(void);

void lin_hardware_timer_init(uint32_t millis, void (*callback)(void*));

#ifdef __cplusplus
}
#endif

#endif

// NRF incluldes
#include "nrfx.h"
#include "nrfx_uart.h"
#include "boards.h"
#include "app_timer.h"

// Local module include
#include "lin_hardware.h"

// UART functions

static nrfx_uart_t uart_driver_instance = NRFX_UART_INSTANCE(0);
static uint8_t rx_buffer[1];
static void (*user_callback)(uint8_t);
static void (*tx_complete)(void);

static void uart_event_handler(nrfx_uart_event_t const *p_event, void *p_context)
{
    if (p_event->type == NRFX_UART_EVT_RX_DONE) {
        if (user_callback) {
            user_callback(rx_buffer[0]);
        }
        // read next byte
        nrfx_uart_rx(&uart_driver_instance, rx_buffer, 1);
    } else if (p_event->type == NRFX_UART_EVT_TX_DONE) {
        if (tx_complete) {
            tx_complete();
        }
    }
}

void lin_hardware_uart_init(uint32_t baudrate, void (*on_receive_callback)(uint8_t))
{
    user_callback = on_receive_callback;

    nrf_uart_baudrate_t nrf_baudrate;
    if (baudrate == 9600) {
        nrf_baudrate = NRF_UART_BAUDRATE_9600;
    } else if (baudrate == 19200) {
        nrf_baudrate = NRF_UART_BAUDRATE_19200;
    } else {
        APP_ERROR_HANDLER(baudrate);
    }

    const nrfx_uart_config_t config = {
        TX_PIN_NUMBER,                      // TXD pin number.
        RX_PIN_NUMBER,                      // RXD pin number.
        NRF_UART_PSEL_DISCONNECTED,         // CTS pin number.
        NRF_UART_PSEL_DISCONNECTED,         // RTS pin number.
        NULL,                               // Context passed to interrupt handler.
        NRF_UART_HWFC_DISABLED,             // Flow control configuration.
        NRF_UART_PARITY_EXCLUDED,           // Parity configuration.
        nrf_baudrate,                       // Baudrate.
        UART_DEFAULT_CONFIG_IRQ_PRIORITY    //< Interrupt priority.
    };

    APP_ERROR_CHECK(nrfx_uart_init(&uart_driver_instance, &config, uart_event_handler));
}

void lin_hardware_uart_uninit(void)
{
    nrfx_uart_uninit(&uart_driver_instance);
}

void lin_hardware_uart_write(const uint8_t *data, size_t length, void (*_tx_complete)(void))
{
    tx_complete = _tx_complete;
    APP_ERROR_CHECK(nrfx_uart_tx(&uart_driver_instance, data, length));
}

void lin_hardware_uart_enable_rx(void)
{
    nrfx_uart_rx_enable(&uart_driver_instance);
    // start reading
    nrfx_uart_rx(&uart_driver_instance, rx_buffer, 1);
}

void lin_hardware_uart_disable_rx(void)
{
    nrfx_uart_rx_disable(&uart_driver_instance);
}

// Timer functions

APP_TIMER_DEF(timer_id);

void lin_hardware_timer_init(uint32_t millis, void (*callback)(void*))
{
    APP_ERROR_CHECK(app_timer_init());
    APP_ERROR_CHECK(app_timer_create(
               &timer_id,
               APP_TIMER_MODE_REPEATED,
               callback));
    APP_ERROR_CHECK(app_timer_start(
               timer_id,
               APP_TIMER_TICKS(millis),
               NULL));
}

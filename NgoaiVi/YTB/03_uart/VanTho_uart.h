#ifndef __VanTho_UART_H
#define __VanTho_UART_H


#include "app_uart.h"
#include "app_error.h"
#include "nrf_delay.h"
#include "nrf.h"
#include "bsp.h"
#if defined (UART_PRESENT)
#include "nrf_uart.h"
#endif
#if defined (UARTE_PRESENT)
#include "nrf_uarte.h"
#endif


#define MAX_TEST_DATA_BYTES     (15U)                /**< max number of test bytes to be used for tx and rx. */
#define UART_TX_BUF_SIZE 256                         /**< UART TX buffer size. */
#define UART_RX_BUF_SIZE 256                         /**< UART RX buffer size. */

typedef void (* uart_handle_t )(uint8_t* data);
void uart_init(uint8_t tx_pin, uint8_t rx_pin, nrf_uart_baudrate_t boudrate);
void uart_set_callback(void *cb);
void uart_put(uint8_t dta);

#endif 
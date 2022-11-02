#include "VanTho_uart.h"

static uart_handle_t uart_handle_main = NULL;

void uart_handle_event(app_uart_evt_t * p_event)
{
	  uint8_t data;
    if(p_event->evt_type == APP_UART_DATA_READY)  
		{
			app_uart_get(&data); // nhan dc data tu may tinh 
			uart_handle_main(&data);
		}	
}


void uart_init(uint8_t tx_pin, uint8_t rx_pin, nrf_uart_baudrate_t boudrate)
{
	 uint32_t err_code;
	 const app_uart_comm_params_t comm_params =
      {
          rx_pin,
          tx_pin,
          RTS_PIN_NUMBER,
          CTS_PIN_NUMBER,
          APP_UART_FLOW_CONTROL_DISABLED,
          false,
					boudrate
			};
			
			APP_UART_FIFO_INIT(&comm_params,
                         UART_RX_BUF_SIZE,
                         UART_TX_BUF_SIZE,
                         uart_handle_event,
                         APP_IRQ_PRIORITY_LOWEST,
                         err_code);

      APP_ERROR_CHECK(err_code);	
}

void uart_put(uint8_t dta)
{
		app_uart_put(dta);
}

void uart_set_callback(void *cb)
{
    if(cb){ 
        uart_handle_main = cb;
    }
}

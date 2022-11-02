
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "VanTho_uart.h"


void HamGoiCallBack(uint8_t* data)
{
	uart_put(*data);
}

int main(void)
{
    
	  uart_init(6,  8,  NRF_UART_BAUDRATE_115200);
		uart_set_callback(HamGoiCallBack);
    while (true)
    {
       
    }
}

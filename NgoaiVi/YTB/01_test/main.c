#include <stdbool.h>
#include <stdint.h>
#include "nrf_delay.h"
#include "boards.h"

#include "NRF52_gpio.h"


int main(void)
{
    nrf52_output_create(17);
  	nrf52_output_set_level(17,0);
    while (true)
    {
       nrf52_toggle(17);
			 nrf_delay_ms(100);
    }
}

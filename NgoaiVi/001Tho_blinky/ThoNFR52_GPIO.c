#include "ThoNFR52_GPIO.h"
#include "nrf_gpio.h"

void nrf52_output_create(uint32_t pin)
{
	nrf_gpio_cfg_output(pin);
}

void nrf52_output_set_level(uint32_t pin, int level)
{
	if(level)
	{
		nrf_gpio_pin_set(pin);
	}
	else
	{
		nrf_gpio_pin_clear(pin);
	}
	
}
void nrf52_toggle(uint32_t pin)
{
	nrf_gpio_pin_toggle(pin);
}
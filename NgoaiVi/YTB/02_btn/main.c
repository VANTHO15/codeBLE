#include <stdbool.h>
#include <stdint.h>
#include "nrf_delay.h"
#include "boards.h"

#include "NRF52_gpio.h"
#include "nrf_drv_gpiote.h"

static void in_pin_handler(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{
	nrf52_toggle(17);
}
void nrf52_button_create(int pin)
{
    ret_code_t err_code;

		if (!nrf_drv_gpiote_is_init()){
				err_code = nrf_drv_gpiote_init();
				APP_ERROR_CHECK(err_code);
		}

    nrf_drv_gpiote_in_config_t in_config = GPIOTE_CONFIG_IN_SENSE_HITOLO(true);
    in_config.pull = NRF_GPIO_PIN_PULLUP;

    err_code = nrf_drv_gpiote_in_init(pin, &in_config, in_pin_handler);
    APP_ERROR_CHECK(err_code);

    nrf_drv_gpiote_in_event_enable(pin, true);
}

int main(void)
{
    nrf52_output_create(17);
  	nrf52_output_set_level(17,0);
	
	  nrf52_button_create(13);
    while (true)
    {

    }
}

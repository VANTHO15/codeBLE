#include <stdbool.h>
#include <stdint.h>
#include "nrf_delay.h"
#include "boards.h"

#include "nrf52_gpio.h"

#include "app_timer.h"
#include "nrf_drv_clock.h"

 
APP_TIMER_DEF(m_blinky_timer_id);    // timer nhay led
 
static void lfclk_request(void)
{
    ret_code_t err_code = nrf_drv_clock_init();
    APP_ERROR_CHECK(err_code);
    nrf_drv_clock_lfclk_request(NULL);
} 
 
static void repeated_timer_handler(void * p_context)
{
    nrf52_output_toggle(17);
}


static void create_timers()
{
    ret_code_t err_code;

    // Create timers
    err_code = app_timer_create(&m_blinky_timer_id,
                                APP_TIMER_MODE_REPEATED,
                                repeated_timer_handler);
    APP_ERROR_CHECK(err_code);
	
}


int main(void)
{
    nrf52_output_create(17);
    nrf52_output_set_level(17, 0);
	
		lfclk_request();
		app_timer_init();
		create_timers();
		ret_code_t err_code = app_timer_start(m_blinky_timer_id, APP_TIMER_TICKS(200), NULL);
		APP_ERROR_CHECK(err_code);
	
    while (true)
    {
			
    }
}

/**
 *@}
 **/

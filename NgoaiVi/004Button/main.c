#include <stdbool.h>
#include <stdint.h>
#include "nrf_delay.h"
#include "boards.h"
#include "nrf52_gpio.h"
#include "app_timer.h"
#include "nrf_drv_clock.h"
 
APP_TIMER_DEF(m_blinky_timer_id);  
APP_TIMER_DEF(m_toggle_timer_id);  
 
static void lfclk_request(void)
{
    ret_code_t err_code = nrf_drv_clock_init();
    APP_ERROR_CHECK(err_code);
    nrf_drv_clock_lfclk_request(NULL);
} 
 
/**@brief Timeout handler for the repeated timer.
 */
static void repeated_timer_handler(void * p_context)  
{
//    nrf52_output_toggle(17);
}

static void toggle_timer_handler(void * p_context)
{
//		nrf52_output_toggle(18);
}


static void create_timers()
{
    ret_code_t err_code;

    // Create timers
    err_code = app_timer_create(&m_blinky_timer_id,
                                APP_TIMER_MODE_REPEATED,
                                repeated_timer_handler);
    APP_ERROR_CHECK(err_code);
	
    // Create timers
    err_code = app_timer_create(&m_toggle_timer_id,
                                APP_TIMER_MODE_REPEATED,
                                toggle_timer_handler);
    APP_ERROR_CHECK(err_code);	
}

extern uint32_t button_press_s;
extern uint32_t button_press_count;
extern bool button_press_flag;

int main(void)
{
    nrf52_output_create(17);
    nrf52_output_create(18);
    nrf52_output_set_level(17, 0);
    nrf52_output_set_level(18, 0);	
	
		nrf52_button_timer_init();
		nrf52_button_create(13);
	
		lfclk_request();
		app_timer_init();
		create_timers();
		ret_code_t err_code = app_timer_start(m_blinky_timer_id, APP_TIMER_TICKS(100), NULL);
		err_code = app_timer_start(m_toggle_timer_id, APP_TIMER_TICKS(500), NULL);
		APP_ERROR_CHECK(err_code);

    /* Toggle LEDs. */
    while (true)
    {
			if(button_press_flag == true)
			{
				if((button_press_s < 1) && (button_press_count == 1)){		// short press  < 1s
					nrf52_output_toggle(17);
				}
				else if(button_press_count == 3)
				{
					nrf52_output_toggle(18);
				}
				else if((button_press_s > 3) && (button_press_count == 1))
				{
					nrf52_output_toggle(17);
					nrf52_output_toggle(18);
				}
				button_press_flag = false;
			}
    }
}


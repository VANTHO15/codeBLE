#include "nrf52_gpio.h"
#include "nrf_gpio.h"
#include "nrf_drv_gpiote.h"
#include "app_timer.h"

APP_TIMER_DEF(m_release_timer_id); 

void nrf52_output_create(uint32_t pin)
{
	nrf_gpio_cfg_output(pin);
}

void nrf52_output_set_level(uint32_t pin, int level)
{
	if(level == 1)
	{
		nrf_gpio_pin_set(pin);
	}
	else{
		nrf_gpio_pin_clear(pin);
	}
}

void nrf52_output_toggle(uint32_t pin)
{
	nrf_gpio_pin_toggle(pin);
}

static uint32_t button_press_start_time;
static uint32_t button_press_time;
static uint32_t button_press_cnt = 0;

uint32_t button_press_s = 0;
uint32_t button_press_count = 0;
bool button_press_flag = false;


static void in_pin_handler(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{
	uint32_t counter = app_timer_cnt_get();
	if(nrf_gpio_pin_read(pin) == 0)	// press
	{
		button_press_cnt++;
		button_press_start_time = counter;
		app_timer_stop(m_release_timer_id);
	}
	else{		// release
		ret_code_t err_code = app_timer_start(m_release_timer_id, APP_TIMER_TICKS(500), NULL);
		APP_ERROR_CHECK(err_code);		
		button_press_time = counter - button_press_start_time;
	}
}

static void tag_button_timeout_handler(void * p_context)
{
	if(button_press_flag == false){
    button_press_s = button_press_time /32768;  // 1 step x time 1 step ( T = 1/ 32768)
		button_press_count = button_press_cnt;
		button_press_flag = true;
	}
	button_press_cnt = 0;
	button_press_time = 0;
}


void nrf52_button_timer_init(void)
{
    // Create timers
    ret_code_t err_code = app_timer_create(&m_release_timer_id,
                                APP_TIMER_MODE_SINGLE_SHOT,
                                tag_button_timeout_handler);
    APP_ERROR_CHECK(err_code);		
}

void nrf52_button_create(int pin)
{
    ret_code_t err_code;

		if (!nrf_drv_gpiote_is_init()){
				err_code = nrf_drv_gpiote_init();
				APP_ERROR_CHECK(err_code);
		}

    nrf_drv_gpiote_in_config_t in_config = GPIOTE_CONFIG_IN_SENSE_TOGGLE(true);
    in_config.pull = NRF_GPIO_PIN_NOPULL;

    err_code = nrf_drv_gpiote_in_init(pin, &in_config, in_pin_handler);
    APP_ERROR_CHECK(err_code);

    nrf_drv_gpiote_in_event_enable(pin, true);
}
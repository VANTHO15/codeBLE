#ifndef __NRF52_GPIO_H
#define	__NRF52_GPIO_H

#include <stdint.h>

void nrf52_output_create(uint32_t pin);
void nrf52_output_set_level(uint32_t pin, int level);
void nrf52_toggle(uint32_t pin);

void nrf52_button_create(int pin);

#endif
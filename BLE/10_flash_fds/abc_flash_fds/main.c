/**
 * Copyright (c) 2017 - 2020, Nordic Semiconductor ASA
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form, except as embedded into a Nordic
 *    Semiconductor ASA integrated circuit in a product or a software update for
 *    such product, must reproduce the above copyright notice, this list of
 *    conditions and the following disclaimer in the documentation and/or other
 *    materials provided with the distribution.
 *
 * 3. Neither the name of Nordic Semiconductor ASA nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * 4. This software, with or without modification, must only be used with a
 *    Nordic Semiconductor ASA integrated circuit.
 *
 * 5. Any software provided in binary form under this license must not be reverse
 *    engineered, decompiled, modified and/or disassembled.
 *
 * THIS SOFTWARE IS PROVIDED BY NORDIC SEMICONDUCTOR ASA "AS IS" AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL NORDIC SEMICONDUCTOR ASA OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include <stdint.h>
#include <string.h>
#include "nrf.h"
#include "nordic_common.h"
#ifdef SOFTDEVICE_PRESENT
#include "nrf_sdh.h"
#include "nrf_sdh_ble.h"
#else
#include "nrf_drv_clock.h"
#endif
#include "app_timer.h"

#define NRF_LOG_MODULE_NAME app
#include "nrf_log.h"
#include "nrf_log_ctrl.h"

#include "flash_user.h"
#include "nrf_delay.h"

/* A tag identifying the SoftDevice BLE configuration. */
#define APP_BLE_CONN_CFG_TAG    1

#ifdef SOFTDEVICE_PRESENT
/**@brief   Function for initializing the SoftDevice and enabling the BLE stack. */
static void ble_stack_init(void)
{
    ret_code_t rc;
    uint32_t   ram_start;

    /* Enable the SoftDevice. */
    rc = nrf_sdh_enable_request();
    APP_ERROR_CHECK(rc);

    rc = nrf_sdh_ble_default_cfg_set(APP_BLE_CONN_CFG_TAG, &ram_start);
    APP_ERROR_CHECK(rc);

    rc = nrf_sdh_ble_enable(&ram_start);
    APP_ERROR_CHECK(rc);
}
#else
static void clock_init(void)
{
    /* Initialize the clock. */
    ret_code_t rc = nrf_drv_clock_init();
    APP_ERROR_CHECK(rc);

    nrf_drv_clock_lfclk_request(NULL);

    /* Wait for the clock to be ready. */
    while (!nrf_clock_lf_is_running()) {;}
}
#endif


/**@brief   Initialize the timer. */
static void timer_init(void)
{
    ret_code_t err_code = app_timer_init();
    APP_ERROR_CHECK(err_code);
}


/**@brief   Initialize logging. */
static void log_init(void)
{
    ret_code_t rc = NRF_LOG_INIT(NULL);
    APP_ERROR_CHECK(rc);
}

/**@brief   Sleep until an event is received. */
static void power_manage(void)
{
#ifdef SOFTDEVICE_PRESENT
    (void) sd_app_evt_wait();
#else
    __WFE();
#endif
}

#define RC_FILE     (0x8010)
#define RC_BOOT_KEY  (0x7010)	//record 1
#define RC_DATA_KEY  (0x7011)	// record 2

uint32_t count_reset = 0;
uint32_t data_len;
uint8_t data_write[10] = {1,2,3,4,5,6,7,8,9,10};
uint8_t data_read[20] = {0};

int main(void)
{
	
		ret_code_t rc;
#ifdef SOFTDEVICE_PRESENT
    ble_stack_init();
#else
    clock_init();
#endif

    timer_init();
    log_init();
		record_init();
		
//		rc = record_read(RC_FILE, RC_BOOT_KEY, (uint8_t*)&count_reset, &data_len);
//		nrf_delay_ms(100);
//		if(rc != NRF_SUCCESS)
//		{
//			record_write(RC_FILE, RC_BOOT_KEY, (uint8_t*)&count_reset, sizeof(uint32_t));
//		}
//		else{
//			count_reset++;
//			NRF_LOG_INFO("reset time %d", count_reset);
//			record_update(RC_FILE, RC_BOOT_KEY, (uint8_t*)&count_reset, sizeof(uint32_t));			
//		}
		
//		nrf_delay_ms(100);
		rc = record_read(RC_FILE, RC_DATA_KEY, data_read, &data_len);
		nrf_delay_ms(100);
		if(rc != NRF_SUCCESS)
		{
			record_write(RC_FILE, RC_DATA_KEY, data_write, sizeof(data_write));
		}
		else{
			data_write[0] = data_read[0] + 10;
			data_write[1] = data_read[1] +10;
			record_update(RC_FILE, RC_DATA_KEY, data_write, sizeof(data_write));			
		}		
		
    /* Enter main loop. */
    for (;;)
    {
        if (!NRF_LOG_PROCESS())
        {
            power_manage();
        }
    }
}


/**
 * @}
 */

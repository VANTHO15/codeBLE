#include "cus_service.h"

#define BLE_NUS_MAX_RX_CHAR_LEN        BLE_NUS_MAX_DATA_LEN /**< Maximum length of the RX Characteristic (in bytes). */
#define BLE_NUS_MAX_TX_CHAR_LEN        BLE_NUS_MAX_DATA_LEN /**< Maximum length of the TX Characteristic (in bytes). */

static void on_write(ble_cus_t * p_cus, ble_evt_t const * p_ble_evt)
{
    ret_code_t                    err_code;
    ble_cus_evt_t                 evt;
    ble_gatts_evt_write_t const * p_evt_write = &p_ble_evt->evt.gatts_evt.params.write;
		evt.conn_handle = p_ble_evt->evt.gatts_evt.conn_handle;
	
		if ((p_evt_write->handle == p_cus->tx_handles.cccd_handle) &&
        (p_evt_write->len == 2))
    {
					if (ble_srv_is_notification_enabled(p_evt_write->data))
					{
							evt.type                          = BLE_CUS_EVT_NOTIFY_ENABLE;
					}
					else
					{
							evt.type                          = BLE_CUS_EVT_NOTIFY_DISABLE;
					}

					if (p_cus->data_handler != NULL)
					{
							p_cus->data_handler(&evt);
					}
    }		
    else if ((p_evt_write->handle == p_cus->rx_handles.value_handle) &&
             (p_cus->data_handler != NULL))
    {
        evt.type                  = BLE_CUS_EVT_RX_DATA;
        evt.params.rx_data.p_data = p_evt_write->data;
        evt.params.rx_data.length = p_evt_write->len;

        p_cus->data_handler(&evt);
    }		
}

static void on_read(ble_cus_t * p_cus, ble_evt_t const * p_ble_evt)
{
	ble_cus_evt_t                 evt;
	ble_gatts_evt_read_t const * p_evt_read = &p_ble_evt->evt.gatts_evt.params.authorize_request.request.read;
	if ((p_evt_read->handle == p_cus->tx_handles.value_handle) &&
							 (p_cus->data_handler != NULL))	
	{
        evt.type                  = BLE_CUS_EVT_READ;
				evt.conn_handle = p_ble_evt->evt.gatts_evt.conn_handle;
        p_cus->data_handler(&evt);		
	}
}

void ble_cus_on_ble_evt(ble_evt_t const * p_ble_evt, void * p_context)
{
		ble_cus_t * p_cus = (ble_cus_t *)p_context;
    switch (p_ble_evt->header.evt_id)
    {
        case BLE_GATTS_EVT_WRITE:
            on_write(p_cus, p_ble_evt);
            break;
				
				case BLE_GATTS_EVT_RW_AUTHORIZE_REQUEST:
						on_read(p_cus, p_ble_evt);
        default:
            // No implementation needed.
            break;
    }	
}

uint32_t ble_cus_init(ble_cus_t * p_cus, ble_cus_init_t const * p_cus_init)
{
	
		ret_code_t            err_code;
    ble_uuid_t            ble_uuid;
    ble_uuid128_t         cus_base_uuid = CUSTOM_SERVICE_UUID_BASE;
    ble_add_char_params_t add_char_params;
	
		p_cus->data_handler = p_cus_init->data_handler;

    VERIFY_PARAM_NOT_NULL(p_cus);
    VERIFY_PARAM_NOT_NULL(p_cus_init);	
    /**@snippet [Adding proprietary Service to the SoftDevice] */
    // Add a custom base UUID.
    err_code = sd_ble_uuid_vs_add(&cus_base_uuid, &p_cus->uuid_type);
    VERIFY_SUCCESS(err_code);

    ble_uuid.type = p_cus->uuid_type;
    ble_uuid.uuid = CUS_SERVICE_UUID;

    // Add the service.
    err_code = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY,
                                        &ble_uuid,
                                        &p_cus->service_handle);
    /**@snippet [Adding proprietary Service to the SoftDevice] */
    VERIFY_SUCCESS(err_code);
	
    // Add the RX Characteristic.
    memset(&add_char_params, 0, sizeof(add_char_params));
    add_char_params.uuid                     = BLE_UUID_NUS_RX_CHARACTERISTIC;
    add_char_params.uuid_type                = p_cus->uuid_type;
    add_char_params.max_len                  = BLE_NUS_MAX_RX_CHAR_LEN;
    add_char_params.init_len                 = sizeof(uint8_t);
    add_char_params.is_var_len               = true;
 
		add_char_params.char_props.write 	= 1;
		
    add_char_params.cccd_write_access = SEC_OPEN;
		add_char_params.read_access       = SEC_OPEN;
		add_char_params.write_access      = SEC_OPEN;	

    err_code = characteristic_add(p_cus->service_handle, &add_char_params, &p_cus->rx_handles);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }	
	
    // Add the TX Characteristic.
    /**@snippet [Adding proprietary characteristic to the SoftDevice] */
    memset(&add_char_params, 0, sizeof(add_char_params));
    add_char_params.uuid              = BLE_UUID_NUS_TX_CHARACTERISTIC;
    add_char_params.uuid_type         = p_cus->uuid_type;
    add_char_params.max_len           = BLE_NUS_MAX_TX_CHAR_LEN;
    add_char_params.init_len          = sizeof(uint8_t);
    add_char_params.is_var_len        = true;
    add_char_params.char_props.notify = 1;
		add_char_params.char_props.read 	= 1;
		add_char_params.is_defered_read 	= 1;		// need for read
		add_char_params.char_props.write 	= 1;

    add_char_params.cccd_write_access = SEC_OPEN;
		add_char_params.read_access       = SEC_OPEN;
		add_char_params.write_access      = SEC_OPEN;		
	
		return characteristic_add(p_cus->service_handle, &add_char_params, &p_cus->tx_handles);
}

uint32_t ble_cus_data_send(ble_cus_t * p_cus,
                           uint8_t   * p_data,
                           uint16_t  * p_length,
                           uint16_t    conn_handle)
{
		ble_gatts_hvx_params_t     hvx_params;
    memset(&hvx_params, 0, sizeof(hvx_params));

    hvx_params.handle = p_cus->tx_handles.value_handle;
    hvx_params.p_data = p_data;
    hvx_params.p_len  = p_length;
    hvx_params.type   = BLE_GATT_HVX_NOTIFICATION;

    return sd_ble_gatts_hvx(conn_handle, &hvx_params);	
}

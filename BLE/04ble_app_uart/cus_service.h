#ifndef __CUS_SERVICE_H
#define __CUS_SERVICE_H

#include <stdint.h>
#include <stdbool.h>
#include "sdk_config.h"
#include "ble.h"
#include "ble_srv_common.h"
#include "nrf_sdh_ble.h"
#include "ble_link_ctx_manager.h"

#ifdef __cplusplus
extern "C" {
#endif

#define BLE_CUS_DEF(_name)                                                                          \
static ble_cus_t _name;                                                                             \
NRF_SDH_BLE_OBSERVER(_name ## _obs,                                                                 \
                     BLE_HRS_BLE_OBSERVER_PRIO,                                                     \
                     ble_cus_on_ble_evt, &_name)

#define CUSTOM_SERVICE_UUID_BASE         {0xBC, 0x8A, 0xBF, 0x45, 0xCA, 0x05, 0x50, 0xBA, \
                                          0x40, 0x42, 0xB0, 0x00, 0xC9, 0xAD, 0x64, 0xF3}

#define CUS_SERVICE_UUID               		0x1400
#define BLE_UUID_NUS_TX_CHARACTERISTIC 0x1401               /**< The UUID of the TX Characteristic. */
#define BLE_UUID_NUS_RX_CHARACTERISTIC 0x1402               /**< The UUID of the RX Characteristic. */		
																					
#define OPCODE_LENGTH        1
#define HANDLE_LENGTH        2

/**@brief   Maximum length of data (in bytes) that can be transmitted to the peer by the Nordic UART service module. */
#if defined(NRF_SDH_BLE_GATT_MAX_MTU_SIZE) && (NRF_SDH_BLE_GATT_MAX_MTU_SIZE != 0)
    #define BLE_NUS_MAX_DATA_LEN (NRF_SDH_BLE_GATT_MAX_MTU_SIZE - OPCODE_LENGTH - HANDLE_LENGTH)
#else
    #define BLE_NUS_MAX_DATA_LEN (BLE_GATT_MTU_SIZE_DEFAULT - OPCODE_LENGTH - HANDLE_LENGTH)
    #warning NRF_SDH_BLE_GATT_MAX_MTU_SIZE is not defined.
#endif

/**@brief   Nordic UART Service event types. */
typedef enum
{
    BLE_CUS_EVT_RX_DATA,      /**< Data received. */
    BLE_CUS_EVT_TX_RDY,       /**< Service is ready to accept new data to be transmitted. */
    BLE_CUS_EVT_NOTIFY_ENABLE, /**< Notification has been enabled. */
    BLE_CUS_EVT_NOTIFY_DISABLE, /**< Notification has been disabled. */
		BLE_CUS_EVT_READ,
} ble_cus_evt_type_t;


/* Forward declaration of the ble_nus_t type. */
typedef struct ble_cus_s ble_cus_t;


/**@brief   Nordic UART Service @ref BLE_NUS_EVT_RX_DATA event data.
 *
 * @details This structure is passed to an event when @ref BLE_NUS_EVT_RX_DATA occurs.
 */
typedef struct
{
    uint8_t const * p_data; /**< A pointer to the buffer with received data. */
    uint16_t        length; /**< Length of received data. */
} ble_cus_evt_rx_data_t;																					
																					
/**@brief   Nordic UART Service event structure.
 *
 * @details This structure is passed to an event coming from service.
 */
typedef struct
{
    ble_cus_evt_type_t         type;        /**< Event type. */
    ble_cus_t                * p_cus;       /**< A pointer to the instance. */
    uint16_t                   conn_handle; /**< Connection handle. */
    union
    {
        ble_cus_evt_rx_data_t rx_data; /**< @ref BLE_NUS_EVT_RX_DATA event data. */
    } params;
} ble_cus_evt_t;


// struct init custom service
typedef void (* ble_cus_data_handler_t) (ble_cus_evt_t * p_evt);

typedef struct
{
    ble_cus_data_handler_t data_handler; /**< Event handler to be called for handling received data. */
} ble_cus_init_t;
																					
// struct thuc the cua custom service
struct ble_cus_s
{
    uint8_t                         uuid_type;          /**< UUID type for Nordic UART Service Base UUID. */
    uint16_t                         service_handle;     /**< Handle of Nordic UART Service (as provided by the SoftDevice). */
    ble_gatts_char_handles_t        tx_handles;         /**< Handles related to the TX characteristic (as provided by the SoftDevice). */
    ble_gatts_char_handles_t        rx_handles;         /**< Handles related to the RX characteristic (as provided by the SoftDevice). */
    ble_cus_data_handler_t          data_handler;       /**< Event handler to be called for handling received data. */
};																					

uint32_t ble_cus_init(ble_cus_t * p_nus, ble_cus_init_t const * p_cus_init);
						
void ble_cus_on_ble_evt(ble_evt_t const * p_ble_evt, void * p_context);

uint32_t ble_cus_data_send(ble_cus_t * p_nus,
                           uint8_t   * p_data,
                           uint16_t  * p_length,
                           uint16_t    conn_handle);

#endif


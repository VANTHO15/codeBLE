#include "flash_user.h"
#include "fds.h"
#include "nrf_log.h"

/* Flag to check fds initialization. */
static bool volatile m_fds_initialized;


const char *fds_err_str(ret_code_t ret)
{
    /* Array to map FDS return values to strings. */
    static char const * err_str[] =
    {
        "FDS_ERR_OPERATION_TIMEOUT",
        "FDS_ERR_NOT_INITIALIZED",
        "FDS_ERR_UNALIGNED_ADDR",
        "FDS_ERR_INVALID_ARG",
        "FDS_ERR_NULL_ARG",
        "FDS_ERR_NO_OPEN_RECORDS",
        "FDS_ERR_NO_SPACE_IN_FLASH",
        "FDS_ERR_NO_SPACE_IN_QUEUES",
        "FDS_ERR_RECORD_TOO_LARGE",
        "FDS_ERR_NOT_FOUND",
        "FDS_ERR_NO_PAGES",
        "FDS_ERR_USER_LIMIT_REACHED",
        "FDS_ERR_CRC_CHECK_FAILED",
        "FDS_ERR_BUSY",
        "FDS_ERR_INTERNAL",
    };

    return err_str[ret - NRF_ERROR_FDS_ERR_BASE];
}


static void fds_evt_handler(fds_evt_t const * p_evt)
{
    switch (p_evt->id)
    {
        case FDS_EVT_INIT:
            if (p_evt->result == NRF_SUCCESS)
            {
                m_fds_initialized = true;
            }
            break;

        case FDS_EVT_WRITE:
        {
            if (p_evt->result == NRF_SUCCESS)
            {
                NRF_LOG_INFO("Record ID:\t0x%04x",  p_evt->write.record_id);
                NRF_LOG_INFO("File ID:\t0x%04x",    p_evt->write.file_id);
                NRF_LOG_INFO("Record key:\t0x%04x", p_evt->write.record_key);
            }
        } break;

        case FDS_EVT_DEL_RECORD:
        {
            if (p_evt->result == NRF_SUCCESS)
            {
                NRF_LOG_INFO("Record ID:\t0x%04x",  p_evt->del.record_id);
                NRF_LOG_INFO("File ID:\t0x%04x",    p_evt->del.file_id);
                NRF_LOG_INFO("Record key:\t0x%04x", p_evt->del.record_key);
            }
        } break;

        default:
            break;
    }
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

/**@brief   Wait for fds to initialize. */
static void wait_for_fds_ready(void)
{
    while (!m_fds_initialized)
    {
			power_manage();
    }
}

ret_code_t record_init(void)
{
		ret_code_t rc;
    NRF_LOG_INFO("FDS started.")

    /* Register first to receive an event when initialization is complete. */
    (void) fds_register(fds_evt_handler);

    NRF_LOG_INFO("Initializing fds...");

    rc = fds_init();
    APP_ERROR_CHECK(rc);

    /* Wait for fds to initialize. */
    wait_for_fds_ready();

    fds_stat_t stat = {0};

    rc = fds_stat(&stat);
    APP_ERROR_CHECK(rc);

    NRF_LOG_INFO("Found %d valid records.", stat.valid_records);
    NRF_LOG_INFO("Found %d dirty records (ready to be garbage collected).", stat.dirty_records);	
		return rc;
}

ret_code_t record_write(uint32_t fid, uint32_t key, uint8_t *data, uint32_t len)
{
		ret_code_t rc;
	
		fds_record_t record = {
				.file_id = fid,
				.key = key,
				.data.p_data = data,
				.data.length_words = BYTES_TO_WORDS(len)
		};
		
		rc = fds_record_write(NULL, &record);
		if ((rc != NRF_SUCCESS) && (rc == FDS_ERR_NO_SPACE_IN_FLASH))
		{
				NRF_LOG_INFO("No space in flash, delete some records to update the config file.");
		}
		else
		{
				APP_ERROR_CHECK(rc);
		}	
}

ret_code_t record_update(uint32_t fid, uint32_t key, uint8_t *data, uint32_t len)
{
		ret_code_t rc;
		
    fds_record_desc_t desc = {0};
    fds_find_token_t  tok  = {0};

    rc = fds_record_find(fid, key, &desc, &tok);

    if (rc == NRF_SUCCESS)
    {
        /* A config file is in flash. Let's update it. */
        fds_flash_record_t config = {0};

				fds_record_t record = {
						.file_id = fid,
						.key = key,
						.data.p_data = data,
						.data.length_words = BYTES_TO_WORDS(len)
				};				
				
        /* Write the updated record to flash. */
        rc = fds_record_update(&desc, &record);
        if ((rc != NRF_SUCCESS) && (rc == FDS_ERR_NO_SPACE_IN_FLASH))
        {
            NRF_LOG_INFO("No space in flash, delete some records to update the config file.");
        }
        else
        {
            APP_ERROR_CHECK(rc);
        }
    }	
		else{
				NRF_LOG_INFO("Not Found record");
		}
		return rc;
}

ret_code_t record_gc(void)
{
		return fds_gc();
}

ret_code_t record_delete(uint32_t fid, uint32_t key)
{
		ret_code_t rc;
		
    fds_record_desc_t desc = {0};
    fds_find_token_t  tok  = {0};

    rc = fds_record_find(fid, key, &desc, &tok);
		if (rc == NRF_SUCCESS)
		{
			rc = fds_record_delete(&desc);
		}
		return rc;
}

ret_code_t record_read(uint32_t fid, uint32_t key, uint8_t *data, uint32_t *len)
{
		ret_code_t rc;
		
    fds_record_desc_t desc = {0};
    fds_find_token_t  tok  = {0};
		fds_flash_record_t record;
    rc = fds_record_find(fid, key, &desc, &tok);
		if (rc == NRF_SUCCESS)
		{
			rc = fds_record_open(&desc, &record);
			*len = record.p_header->length_words*BYTES_PER_WORD;
			memcpy(data, record.p_data, *len);
			rc = fds_record_close(&desc);
		}	
		return rc;
}
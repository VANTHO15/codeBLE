#ifndef __FLASH_USER_H
#define __FLASH_USER_H
#include <stdint.h>
#include "app_error.h"
ret_code_t record_init(void);
ret_code_t record_write(uint32_t fid, uint32_t key, uint8_t *data, uint32_t len);
ret_code_t record_update(uint32_t fid, uint32_t key, uint8_t *data, uint32_t len);
ret_code_t record_gc(void);
ret_code_t record_delete(uint32_t fid, uint32_t key);
ret_code_t record_read(uint32_t fid, uint32_t key, uint8_t *data, uint32_t *len);
#endif
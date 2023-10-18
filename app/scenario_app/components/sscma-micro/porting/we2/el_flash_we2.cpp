/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2023 (Seeed Technology Inc.)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */

#include "el_flash_we2.h"

#include "core/el_debug.h"
#include "spi_eeprom_comm.h"

namespace edgelab {

el_err_code_t el_model_partition_mmap_init(const char*              partition_name,
                                           uint32_t*                partition_start_addr,
                                           uint32_t*                partition_size,
                                           const uint8_t**          flash_2_memory_map,
                                           el_model_mmap_handler_t* mmap_handler) {
    *partition_start_addr = 0x00;
    *partition_size       = 0x400000;

    hx_lib_spi_eeprom_open(USE_DW_SPI_MST_Q);
    hx_lib_spi_eeprom_enable_XIP(USE_DW_SPI_MST_Q, true, FLASH_QUAD, true);

    *flash_2_memory_map = (const uint8_t*)0x3A400000;

    return EL_OK;
}

void el_model_partition_mmap_deinit(el_model_mmap_handler_t* mmap_handler) {}

#ifdef CONFIG_EL_LIB_FLASHDB

static int el_flash_db_init(void) {
    hx_lib_spi_eeprom_open(USE_DW_SPI_MST_Q);
    hx_lib_spi_eeprom_enable_XIP(USE_DW_SPI_MST_Q, true, FLASH_QUAD, true);
    return 1;
}

static int el_flash_db_read(long offset, uint8_t* buf, size_t size) {
    int8_t   ret  = 0;
    uint32_t addr = el_flash_db_nor_flash0.addr + offset;
    if (addr + size >= 0x800000) {
        return -1;
    }

    memcpy(buf, (uint8_t*)(0x3A000000 + addr), size);

    return ret;
}

static int el_flash_db_write(long offset, const uint8_t* buf, size_t size) {
    int8_t   ret  = 0;
    uint32_t addr = el_flash_db_nor_flash0.addr + offset;
    if (addr + size >= 0x800000) {
        return -1;
    }
    hx_lib_spi_eeprom_enable_XIP(USE_DW_SPI_MST_Q, false, FLASH_QUAD, false);
    hx_lib_spi_eeprom_word_write(USE_DW_SPI_MST_Q, addr, (uint32_t*)buf, size);
    hx_lib_spi_eeprom_enable_XIP(USE_DW_SPI_MST_Q, true, FLASH_QUAD, true);
    return ret;
}

static int el_flash_db_erase(long offset, size_t size) {
    int8_t   ret  = 0;
    uint32_t addr = el_flash_db_nor_flash0.addr + offset;

    if (addr + size >= 0x800000) {
        return -1;
    }

    hx_lib_spi_eeprom_enable_XIP(USE_DW_SPI_MST_Q, false, FLASH_QUAD, false);
    hx_lib_spi_eeprom_erase_sector(USE_DW_SPI_MST_Q, addr, FLASH_SECTOR);
    hx_lib_spi_eeprom_enable_XIP(USE_DW_SPI_MST_Q, true, FLASH_QUAD, true);

    return ret;
}

const struct fal_flash_dev el_flash_db_nor_flash0 = {
  .name       = CONFIG_EL_STORAGE_PARTITION_MOUNT_POINT,
  .addr       = 0x800000 - 0x10000,
  .len        = 0x10000,
  .blk_size   = FDB_BLOCK_SIZE,
  .ops        = {el_flash_db_init, el_flash_db_read, el_flash_db_write, el_flash_db_erase},
  .write_gran = FDB_WRITE_GRAN,
};
#endif

}  // namespace edgelab

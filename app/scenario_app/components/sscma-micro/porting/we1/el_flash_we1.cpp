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

#include "el_flash_we1.h"

#include "core/el_debug.h"

namespace edgelab {

el_err_code_t el_model_partition_mmap_init(const char*              partition_name,
                                           uint32_t*                partition_start_addr,
                                           uint32_t*                partition_size,
                                           const uint8_t**          flash_2_memory_map,
                                           el_model_mmap_handler_t* mmap_handler) {
#ifdef FLASH_ENABLE_PIN
    hx_drv_iomux_set_pmux(FLASH_ENABLE_PIN, 3);
    hx_drv_iomux_set_outvalue(FLASH_ENABLE_PIN, FLASH_ENABLE_STATE);
#endif
    DEV_SPI_PTR dev_spi_m;
    dev_spi_m = hx_drv_spi_mst_get_dev(USE_DW_SPI_MST_1);
    dev_spi_m->spi_open(DEV_MASTER_MODE, 50000000, 400000000);  // master mode, spiclock, cpuclock
    dev_spi_m->flash_set_xip(true, SPI_M_MODE_QUAD);

    *partition_start_addr = 0x00;
    *partition_size       = 0x400000;
    *flash_2_memory_map   = (const uint8_t*)0x30000000;

    return EL_OK;
}

void el_model_partition_mmap_deinit(el_model_mmap_handler_t* mmap_handler) {}

static int el_flash_db_init(void) {
    if (hx_drv_spi_flash_open(0) != 0) {
        return -1;
    }

    if (hx_drv_spi_flash_open_speed(50000000, 400000000) != 0) {
        return -1;
    }

    //el_printf("flash db size: 0x%08x\n", CONFIG_EL_STORAGE_PARTITION_FS_SIZE_0);

    return 0;
}

static int el_flash_db_read(long offset, uint8_t* buf, size_t size) {
    int8_t   ret  = 0;
    uint32_t addr = el_flash_db_nor_flash0.addr + offset;

    //el_printf("flash db read 0x%08x 0x%08x\n", addr, size);

    if (addr >= INTERNAL_FLASH_SIZE) {
        return -1;
    }

    if (hx_drv_spi_flash_protocol_read(0, addr, (uint32_t)buf, size, 4) != 0) {
        ret = -1;
    }
    return ret;
}

static int el_flash_db_write(long offset, const uint8_t* buf, size_t size) {
    int8_t   ret  = 0;
    uint32_t addr = el_flash_db_nor_flash0.addr + offset;
    //el_printf("flash db write 0x%08x 0x%08x\n", addr, size);
    if (addr >= INTERNAL_FLASH_SIZE) {
        return -1;
    }

    if (hx_drv_spi_flash_protocol_write(0, addr, (uint32_t)buf, size, 4) != 0) {
        ret = -1;
    }

    return ret;
}

static int el_flash_db_erase(long offset, size_t size) {
    int8_t   ret  = 0;
    uint32_t addr = el_flash_db_nor_flash0.addr + offset;
    // el_printf("flash db erase 0x%08x 0x%08x\n", addr, size);
    uint32_t sector_num = size / INTERNAL_FLASH_SECTOR_SIZE + (size % INTERNAL_FLASH_SECTOR_SIZE ? 1 : 0);
    for (int i = 0; i < sector_num; i++) {
        uint32_t sector_addr = addr + i * INTERNAL_FLASH_SECTOR_SIZE;
        // el_printf("flash db erase 0x%08x size: %d / %d\n", sector_addr, i, sector_num);
        if (sector_addr >= INTERNAL_FLASH_SIZE) {
            return -1;
        }
        if (hx_drv_spi_flash_protocol_erase_sector(0, sector_addr) != 0) {
            return -1;
        }
    }

    return ret;
}

#ifdef CONFIG_EL_LIB_FLASHDB
const struct fal_flash_dev el_flash_db_nor_flash0 = {
  {.name = CONFIG_EL_STORAGE_PARTITION_MOUNT_POINT},
 //0x200000 is the flash size of himax6538, 0x10000 is the data size for flashdb
  .addr       = (INTERNAL_FLASH_SIZE - CONFIG_EL_STORAGE_PARTITION_FS_SIZE_0),
  .len        = CONFIG_EL_STORAGE_PARTITION_FS_SIZE_0,
  .blk_size   = FDB_BLOCK_SIZE,
  .ops        = {el_flash_db_init, el_flash_db_read, el_flash_db_write, el_flash_db_erase},
  .write_gran = FDB_WRITE_GRAN,
};
#endif

}  // namespace edgelab
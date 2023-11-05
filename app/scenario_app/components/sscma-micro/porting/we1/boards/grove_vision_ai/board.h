/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2023 Hongtai Liu (Seeed Technology Inc.)
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

#ifndef _GROVE_VISION_AI_H
#define _GROVE_VISION_AI_H

#include "embARC_debug.h"
#include "hx_drv_timer.h"

#define I2C_SYNC_PIN          IOMUX_PGPIO0
#define I2C_SYNC_STATE        1

#define WEBUSB_SYNC_PIN       IOMUX_PGPIO8
#define WEBUSB_SYNC_STATE     1

#define DEBUGGER_ATTACH_PIN   IOMUX_PGPIO1
#define DEBUGGER_ATTACH_STATE 1

#define USE_CAMERA
#define OV2640_CAMERA

#define SENSOR_WIDTH_DEFAULT  192
#define SENSOR_HEIGHT_DEFAULT 192

#define IGNORE_FIRST_X_PICS 2

//#define USE_IMU
//#define IMU_LSM6DS3

#define CMD_IMU
#define CMD_GPIO

#define CMD_READ_CRC
#define CMD_WRITE_CRC

#define CAMERA_ENABLE_PIN           IOMUX_PGPIO9
#define CAMERA_ENABLE_STATE         1

#define VISION_ROTATION             ROTATION_UP

#define INTERNAL_FLASH_BASE_ADDRESS 0x00000000
#define INTERNAL_FLASH_SIZE         0x200000  // 2MB
#define INTERNAL_FLASH_SECTOR_SIZE  0x1000

#endif

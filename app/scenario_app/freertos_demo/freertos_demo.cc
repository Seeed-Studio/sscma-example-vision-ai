/*
 * Copyright (c) 2023 EdgeImpulse Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an "AS
 * IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied. See the License for the specific language
 * governing permissions and limitations under the License.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Include ----------------------------------------------------------------- */
#include "freertos_demo.h"

#include <cstdio>
#include <cstdlib>

#include "FreeRTOS.h"
#include "board_config.h"
#include "embARC.h"
#include "embARC_debug.h"
#include "hx_drv_timer.h"
#include "hx_drv_uart.h"
#include "semphr.h"
#include "task.h"

xSemaphoreHandle mutex;

static void task1(void* pvParameters) {
    while (1) {
        xSemaphoreTake(mutex, portMAX_DELAY);
        EMBARC_PRINTF("+\r\n");
        xSemaphoreGive(mutex);
        vTaskDelay(500 / portTICK_PERIOD_MS);
    }
}

static void task2(void* pvParameters) {
    while (1) {
        xSemaphoreTake(mutex, portMAX_DELAY);
        EMBARC_PRINTF("-\r\n");
        xSemaphoreGive(mutex);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

static void task3(void* pvParameters) {
    while (1) {
        xSemaphoreTake(mutex, portMAX_DELAY);
        uint8_t uart_buffer[32];
        EMBARC_PRINTF("qquart_buffer:0x%08x\r\n", uart_buffer);
        EMBARC_PRINTF("*\r\n");
        xSemaphoreGive(mutex);
        vTaskDelay(3000 / portTICK_PERIOD_MS);
    }
}

int freertos_demo(void) {
    EMBARC_PRINTF("freertos_demo!\r\n");

    uint8_t uart_buffer[32];
    EMBARC_PRINTF("uart init:0x%08x\r\n", uart_buffer);

    mutex = xSemaphoreCreateMutex();

    EMBARC_PRINTF("creating freertos task1\r\n");

    xTaskCreate(task1, "Task 1", 1024, NULL, 1, NULL);

    EMBARC_PRINTF("creating freertos task2\r\n");

    xTaskCreate(task2, "Task 2", 1024, NULL, 1, NULL);

    EMBARC_PRINTF("creating freertos task3\r\n");

    xTaskCreate(task3, "Task 3", 1024, NULL, 1, NULL);

    EMBARC_PRINTF("starting freertos tasks\r\n");

    vTaskStartScheduler();

    for (;;) {
        EMBARC_PRINTF("Hello World!\r\n");
        board_delay_ms(1000);
    }
}

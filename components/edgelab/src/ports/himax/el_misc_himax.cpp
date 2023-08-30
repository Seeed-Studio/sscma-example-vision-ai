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

#include "el_common.h"

#include "hx_drv_timer.h"
#include "logger.h"
#include "console_io.h"
#include <stdlib.h>

namespace edgelab {

EL_ATTR_WEAK void el_sleep(uint32_t ms) { board_delay_ms(ms); }

EL_ATTR_WEAK uint64_t el_get_time_ms(void) { return (uint64_t)board_get_cur_us() / 1000; }

EL_ATTR_WEAK uint64_t el_get_time_us(void) { return (uint64_t)board_get_cur_us(); }

//TODO: need a implement
EL_ATTR_WEAK int el_printf(const char* fmt, ...) {  }

EL_ATTR_WEAK int el_putchar(char c) { return console_putchar(c); }

EL_ATTR_WEAK void* el_malloc(size_t size) { return malloc(size); }

EL_ATTR_WEAK void* el_calloc(size_t nmemb, size_t size) { return calloc(nmemb, size); }

EL_ATTR_WEAK void el_free(void* ptr) { free(ptr); }

EL_ATTR_WEAK void el_reset(void) { exit(0); }

}  // namespace edgelab
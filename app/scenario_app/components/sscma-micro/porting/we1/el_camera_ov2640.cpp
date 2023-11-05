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

#include "el_camera_ov2640.h"

#include <camera_core.h>
#include <datapath.h>

namespace edgelab {

el_err_code_t CameraOV2640::init(size_t width, size_t height) {
    el_err_code_t ret = EL_OK;

    ret = datapath_init(width, height);
    if (ret != EL_OK) {
        return EL_ELOG;
    }

    ret = camera_init(width, height);
    if (ret != EL_OK) {
        return EL_ELOG;
    }

    datapath_start_work();

    return EL_OK;
}

el_err_code_t CameraOV2640::deinit() {
    datapath_stop_work();
    camera_deinit();
    return EL_OK;
}

el_err_code_t CameraOV2640::start_stream() {
    this->_is_streaming = true;

    if (datapath_capture(2000) != EL_OK) {
        this->_is_streaming = false;
        return EL_EIO;
    }
    return EL_OK;
}

el_err_code_t CameraOV2640::stop_stream() {
    //datapath_stop_work();
    this->_is_streaming = false;
    return EL_OK;
}

el_err_code_t CameraOV2640::get_frame(el_img_t* img) {
    if (!this->_is_streaming) {
        return EL_EIO;
    }

    *img = datapath_get_frame();

    el_printf("get_frame: %d %d %d %d\n", img->width, img->height, img->size, img->format);

    return EL_OK;
}

el_err_code_t CameraOV2640::get_jpeg(el_img_t* img) {
    if (!this->_is_streaming) {
        return EL_EIO;
    }

    *img = datapath_get_jpeg();

    el_printf("get_jpeg: %d %d %d %d\n", img->width, img->height, img->size, img->format);

    return EL_OK;
}

}  // namespace edgelab

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

#include "el_camera_himax.h"

namespace edgelab {
el_err_code_t CameraHimax::init(size_t width, size_t height) {

    ERROR_T ret = ERROR_NONE;
    Sensor_Cfg_t sensor_cfg_t = {
        .sensor_type = SENSOR_CAMERA,
        .data.camera_cfg.width = (uint16_t)width,
        .data.camera_cfg.height = (uint16_t)height,
    };

    ret = datapath_init(sensor_cfg_t.data.camera_cfg.width,
                        sensor_cfg_t.data.camera_cfg.height);
    if (ret != ERROR_NONE)
    {
        return EL_ELOG;
    }
    ret = sensor_init(&sensor_cfg_t);
    if (ret != ERROR_NONE)
    {
        return EL_ELOG;
    }

    return EL_OK;    
}

el_err_code_t CameraHimax::deinit() {
    camera_deinit();
    return EL_OK;
}

el_err_code_t CameraHimax::start_stream() {
    datapath_start_work();

    this->_is_streaming = true;
    return EL_OK;
}

el_err_code_t CameraHimax::stop_stream() {
    camera_deinit();
    this->_is_streaming = false;
    return EL_OK;
}

el_err_code_t CameraHimax::get_frame(el_img_t* img) { return EL_OK; }

el_err_code_t CameraHimax::get_jpeg(el_img_t* img) {
    if (!this->_is_streaming) {
        return EL_EIO;
    }

    volatile uint32_t jpeg_addr;
    volatile uint32_t jpeg_size;
    datapath_get_jpeg_img(&jpeg_addr, &jpeg_size);

    img->width  = this->config.data.camera_cfg.width;
    img->height = this->config.data.camera_cfg.height;
    img->data   = jpeg_addr;
    img->size   = jpeg_size;
    img->format = EL_PIXEL_FORMAT_RGB565;

    return EL_OK;
}
el_err_code_t CameraHimax::get_resolutions(el_res_t** res, size_t* res_count) { return EL_OK; }

}  // namespace edgelab

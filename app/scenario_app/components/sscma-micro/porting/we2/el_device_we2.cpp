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

#include "el_device_we2.h"

#include "porting/we2/el_camera_ov5647.h"
#include "porting/we2/el_serial_we2.h"

namespace edgelab {

bool DeviceWE2::_is_first_run = true;

#define U55_BASE BASE_ADDR_APB_U55_CTRL_ALIAS

struct ethosu_driver ethosu_drv; /* Default Ethos-U device driver */

static void _arm_npu_irq_handler(void) {
    /* Call the default interrupt handler from the NPU driver */
    ethosu_irq_handler(&ethosu_drv);
}

/**
 * @brief  Initialises the NPU IRQ
 **/
static void _arm_npu_irq_init(void) {
    const IRQn_Type ethosu_irqnum = (IRQn_Type)U55_IRQn;

    /* Register the EthosU IRQ handler in our vector table.
     * Note, this handler comes from the EthosU driver */
    EPII_NVIC_SetVector(ethosu_irqnum, (uint32_t)_arm_npu_irq_handler);

    /* Enable the IRQ */
    NVIC_EnableIRQ(ethosu_irqnum);
}

static int _arm_npu_init(bool security_enable, bool privilege_enable) {
    int err = 0;

    /* Initialise the IRQ */
    _arm_npu_irq_init();

    /* Initialise Ethos-U55 device */
    const void* ethosu_base_address = (void*)(U55_BASE);

    if (0 != (err = ethosu_init(&ethosu_drv,          /* Ethos-U driver device pointer */
                                ethosu_base_address,  /* Ethos-U NPU's base address. */
                                NULL,                 /* Pointer to fast mem area - NULL for U55. */
                                0,                    /* Fast mem region size. */
                                security_enable,      /* Security enable. */
                                privilege_enable))) { /* Privilege enable. */
        el_printf("failed to initalise Ethos-U device\n");
        return err;
    }

    el_printf("Ethos-U55 device initialised\n");

    return 0;
}

DeviceWE2::DeviceWE2() {
    this->_device_name = "Grove Vision AI (WE-II)";
    this->_device_id   = 0x0001;
    this->_revision_id = 0x0001;
    static CameraOV5647 camera{};
    static SerialWE2    serial{};
    this->_camera = &camera;
    this->_serial = &serial;
}

DeviceWE2::~DeviceWE2() {}

Device* Device::get_device() {
    static DeviceWE2 device;

    if (DeviceWE2::_is_first_run) {
        uint32_t wakeup_event;
        uint32_t wakeup_event1;

        hx_drv_pmu_get_ctrl(PMU_pmu_wakeup_EVT, &wakeup_event);
        hx_drv_pmu_get_ctrl(PMU_pmu_wakeup_EVT1, &wakeup_event1);
        el_printf("wakeup_event=0x%x,WakeupEvt1=0x%x\n", wakeup_event, wakeup_event1);

        _arm_npu_init(true, true);

        el_printf("DeviceWE2::init()\n");
        DeviceWE2::_is_first_run = false;
    }

    return &device;
}

void DeviceWE2::restart() {}

}  // namespace edgelab

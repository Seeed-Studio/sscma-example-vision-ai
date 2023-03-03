#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include <forward_list>

#include "grove_ai_config.h"
#include "i2c_handlers.h"

#ifdef __cplusplus
extern "C"
{
#endif
#include "hx_drv_iomux.h"
#include "hx_drv_timer.h"
#include "powermode.h"
#include "i2c_comm.h"
#include "internal_flash.h"
#include "hx_drv_webusb.h"
#ifdef __cplusplus
}
#endif

/* Exported macros ---------------------------------------------------------*/
#define GROVE_AI_ADDRESS (0x62)

#define GROVE_AI_CAMERA_ID 0x0100

#define FEATURE_SYSTEM 0x80
#define FEATURE_ALGO 0xA0

#define FEATURE_INVAILD 0xFF
#define CMD_INVAILD 0xFF

#define CMD_SYS_READ_VERSION 0x01
#define CMD_SYS_VERSION_LENGTH 2
#define CMD_SYS_READ_ID 0x02
#define CMD_SYS_ID_LENGTH 2
#define CMD_SYS_READ_STATE 0x03
#define CMD_SYS_STATE_LENGTH 1
#define CMD_SYS_READ_ERROR_CODE 0x04
#define CMD_SYS_CODE_LENGTH 1
#define CMD_SYS_RESET 0x20

#define CMD_ALGO_READ_ALGO 0x00
#define CMD_ALGO_WRITE_ALGO 0x01
#define CMD_ALGO_ALGO_LENGTH 0x01
#define CMD_ALGO_READ_MODEL 0x10
#define CMD_ALGO_WRITE_MODEL 0x11
#define CMD_ALGO_MODEL_LENGTH 0x01
#define CMD_ALGO_READ_VALID_MODEL 0x12
#define CMD_ALGO_VALID_MODEL_LENGTH 0x04
#define CMD_ALGO_READ_PERIOD 0x20
#define CMD_ALGO_WRITE_PERIOD 0x21
#define CMD_ALGO_PERIOD_LENGTH 0x04
#define CMD_ALGO_READ_CONFIDENCE 0x40
#define CMD_ALGO_WRITE_CONFIDENCE 0x41
#define CMD_ALGO_CONFIDENCE_LENGTH 0x01
#define CMD_ALGO_READ_IOU 0x50
#define CMD_ALGO_WRITE_IOU 0x51
#define CMD_ALGO_IOU_LENGTH 0x01
#define CMD_ALGO_INOVKE 0xA0
#define CMD_ALGO_READ_RET_LEN 0xA1
#define CMD_ALGO_READ_RET_LEN_LENGTH 0x02
#define CMD_ALGO_READ_RET 0xA2
#define CMD_ALGO_CONFIG_SAVE 0xEE
#define CMD_ALGO_CONFIG_CLEAR 0xEF

typedef enum
{
    ALGO_OBJECT_DETECTION = 0,
    ALGO_OBJECT_COUNTING = 1,
    ALGO_MAX,
} ALGO_INDEX_T;

typedef enum
{
    MODEL_PRE_INDEX_1 = 0x00,
    MODEL_EXT_INDEX_1 = 0x01,
    MODEL_EXT_INDEX_2 = 0x02,
    MODEL_EXT_INDEX_3 = 0x03,
    MODEL_EXT_INDEX_4 = 0x04,
    MODEL_MAX = 0x20,
} MODEL_INDEX_T;

typedef enum
{
    CMD_ALGO_INVOKE_STOP = 0x00,
    CMD_ALGO_INVOKE_START = 0x01
} CMD_ALGO_STATE_T;

/* config file */
#define ALGO_CONFIG_MAGIC1 0xc2d25ec8
#define ALGO_CONFIG_MAGIC2 0xfe3cda40

typedef struct
{
    uint32_t start_magic1;
    uint32_t start_magic2;
    uint8_t algo;
    uint8_t model;
    uint8_t invoke;
    uint32_t period;
    uint8_t confidence;
    uint8_t iou;
    uint16_t ret_len;
    uint32_t end_magic1;
    uint32_t end_magic2;
} algoConfig_t;

static I2CServer *i2c = NULL;
static algoConfig_t _algoConfig = {0};

#define CMD_HEADER_LENGTH 0x02

/* Live preview -------------------------------------------------------*/

#define PREVIEW_MAX_SIZE 1024

// Object detection
#define DETECTION_PREVIEW_MAX_SIZE 10
#define DETECTION_PR VEW_ELEMENT_NUM 6
#define DETECTION_PR VEW_ELEMENT_SIZE 4
#define DETECTION_PREVIEW_FORMATE "{\"type\":\"preview\", \"algorithm\":%d, \"model\":%d, \"count\":%d, \"object\":{\"x\": [%s],\"y\": [%s],\"w\": [%s],\"h\": [%s],\"target\": [%s],\"confidence\": [%s]}}"

// Object counting
#define COUNTING_PREVIEW_MAX_SIZE 10
#define COUNTING_PR VEW_ELEMENT_NUM 2
#define COUNTING_PR VEW_ELEMENT_SIZE 4
#define COUNTING_PREVIEW_FORMATE "{\"type\":\"preview\", \"algorithm\":%d, \"model\":%d,\"count\":%d, \"object\":{\"target\": [%s],\"count\": [%s]}}"

/* Private variable -------------------------------------------------------*/
static uint8_t i2c_read_buf[I2CCOMM_MAX_WBUF_SIZE] = {0};
static uint8_t i2c_write_buf[I2CCOMM_MAX_RBUF_SIZE] = {0};
static char _preview_buf[PREVIEW_MAX_SIZE] = {0};

/**
 * @brief  i2c event irq handle
 * @param  *param
 * @retval none
 */
static void i2c_event_rx_cb(void *param)
{
    (void)param;

#ifdef I2C_SYNC_PIN
    // set I2C sync pin to busy
    hx_drv_iomux_set_outvalue(I2C_SYNC_PIN, 1 - I2C_SYNC_STATE);
#endif

    uint8_t feature = i2c_read_buf[I2CFMT_FEATURE_OFFSET];
    uint8_t cmd = i2c_read_buf[I2CFMT_COMMAND_OFFSET];

    memset(i2c_write_buf, 0x00, I2CCOMM_MAX_RBUF_SIZE);

    i2c->onService(feature, cmd, i2c_read_buf + CMD_HEADER_LENGTH, i2c_write_buf);

end:
    memset(i2c_read_buf, 0x00, CMD_HEADER_LENGTH);

    hx_lib_i2ccomm_enable_write(i2c_write_buf);
    hx_lib_i2ccomm_enable_read(i2c_read_buf, I2CCOMM_MAX_RBUF_SIZE);

#ifdef I2C_SYNC_PIN
    // set I2C sync pin to idle
    hx_drv_iomux_set_outvalue(I2C_SYNC_PIN, I2C_SYNC_STATE);
#endif
    return;
}

static void i2c_event_tx_cb(void *param)
{
    (void)param;
}
static void i2c_event_err_cb(void *param)
{
    (void)param;
    memset(i2c_read_buf, 0xFF, CMD_HEADER_LENGTH);
    hx_lib_i2ccomm_enable_read(i2c_read_buf, I2CCOMM_MAX_RBUF_SIZE);
}

I2CState_t i2c_read_version(const uint8_t *read_buf, uint8_t *write_buf, uint8_t *len)
{
    write_buf[0] = GROVE_AI_FAMILY_MAIN_VER;
    write_buf[1] = GROVE_AI_FAMILY_SUB_VER;

    *len = CMD_SYS_VERSION_LENGTH;

    return I2CState_t::I2C_IDLE;
}

I2CState_t i2c_read_id(const uint8_t *read_buf, uint8_t *write_buf, uint8_t *len)
{
    write_buf[0] = GROVE_AI_FAMILY_MAIN_ID;
    write_buf[1] = GROVE_AI_FAMILY_SUB_ID;

    *len = CMD_SYS_ID_LENGTH;

    return I2CState_t::I2C_IDLE;
}

I2CState_t i2c_read_state(const uint8_t *read_buf, uint8_t *write_buf, uint8_t *len)
{
    write_buf[0] = i2c->get_state();

    *len = CMD_SYS_STATE_LENGTH;

    return I2CState_t::I2C_IDLE;
}

void i2c_algo_init()
{
    memset(&_algoConfig, 0, sizeof(_algoConfig));
    // read algo config from flash
    if (internal_flash_read(ALGO_CONFIG_ADDR, &_algoConfig, sizeof(_algoConfig)) == 0)
    {
        // check magic number
        if (_algoConfig.start_magic1 != ALGO_CONFIG_MAGIC1 || _algoConfig.start_magic2 != ALGO_CONFIG_MAGIC2 ||
            _algoConfig.end_magic1 != ALGO_CONFIG_MAGIC1 || _algoConfig.end_magic2 != ALGO_CONFIG_MAGIC2)
        {
            // write default config to flash
            _algoConfig.algo = ALGO_OBJECT_DETECTION;
            _algoConfig.model = MODEL_EXT_INDEX_1;
            _algoConfig.period = 0;
            _algoConfig.ret_len = 0;
            _algoConfig.invoke = 0;
            _algoConfig.confidence = 50;
            _algoConfig.iou = 45;

            _algoConfig.start_magic1 = ALGO_CONFIG_MAGIC1;
            _algoConfig.start_magic2 = ALGO_CONFIG_MAGIC2;
            _algoConfig.end_magic1 = ALGO_CONFIG_MAGIC1;
            _algoConfig.end_magic2 = ALGO_CONFIG_MAGIC2;

            internal_flash_write(ALGO_CONFIG_ADDR, &_algoConfig, sizeof(_algoConfig));
        }
    }
    else
    {
        // write default config
        _algoConfig.algo = ALGO_OBJECT_DETECTION;
        _algoConfig.model = MODEL_EXT_INDEX_1;
        _algoConfig.period = 0;
        _algoConfig.invoke = 0;
        _algoConfig.ret_len = 0;
        _algoConfig.confidence = 50;
        _algoConfig.iou = 45;
    }
}

I2CState_t i2c_reset()
{

#ifdef EXTERNAL_LDO
    hx_lib_pm_chip_rst(PMU_WE1_POWERPLAN_EXTERNAL_LDO);
#else
    hx_lib_pm_chip_rst(PMU_WE1_POWERPLAN_INTERNAL_LDO);
#endif

    return I2CState_t::I2C_IDLE;
}

I2CState_t i2c_read_algo(const uint8_t *read_buf, uint8_t *write_buf, uint8_t *len)
{
    write_buf[0] = _algoConfig.algo;

    *len = CMD_ALGO_ALGO_LENGTH;

    return I2CState_t::I2C_IDLE;
}

I2CState_t i2c_write_algo(const uint8_t *read_buf, uint8_t *write_buf)
{
    uint8_t algo = read_buf[0];

    if (algo < ALGO_MAX)
    {
        _algoConfig.algo = algo;
    }

    return I2CState_t::I2C_IDLE;
}

I2CState_t i2c_read_model(const uint8_t *read_buf, uint8_t *write_buf, uint8_t *len)
{
    write_buf[0] = _algoConfig.model;

    *len = CMD_ALGO_MODEL_LENGTH;

    return I2CState_t::I2C_IDLE;
}

I2CState_t i2c_write_model(const uint8_t *read_buf, uint8_t *write_buf)
{
    // !!!   MODEL COULDENT BE CHANGED
    _algoConfig.model = MODEL_EXT_INDEX_1;

    return I2CState_t::I2C_IDLE;
}

I2CState_t i2c_read_valid_model(const uint8_t *read_buf, uint8_t *write_buf, uint8_t *len)
{
    // !!!   VALID MODEL IS ALWAYS  1 << MODEL_EXT_INDEX_1
    write_buf[0] = 1 << MODEL_EXT_INDEX_1;

    *len = CMD_ALGO_ALGO_LENGTH;

    return I2CState_t::I2C_IDLE;
}

I2CState_t i2c_read_confidence(const uint8_t *read_buf, uint8_t *write_buf, uint8_t *len)
{

    write_buf[0] = _algoConfig.confidence;

    *len = CMD_ALGO_CONFIDENCE_LENGTH;

    return I2CState_t::I2C_IDLE;
}

I2CState_t i2c_write_confidence(const uint8_t *read_buf, uint8_t *write_buf)
{
    uint8_t confidence = read_buf[0];

    if (confidence <= 100)
    {
        _algoConfig.confidence = confidence;
    }
    else
    {
        _algoConfig.confidence = 100;
    }

    return I2CState_t::I2C_IDLE;
}

I2CState_t i2c_read_ret_len(const uint8_t *read_buf, uint8_t *write_buf, uint8_t *len)
{

    write_buf[0] = (_algoConfig.ret_len >> 8) & 0xFF;
    write_buf[1] = _algoConfig.ret_len & 0xFF;

    *len = CMD_ALGO_READ_RET_LEN_LENGTH;

    return I2CState_t::I2C_IDLE;
}

I2CState_t i2c_read_ret(const uint8_t *read_buf, uint8_t *write_buf, uint8_t *len)
{

    uint16_t index = ((uint16_t)read_buf[0] << 8) + read_buf[1];

    if (index >= _algoConfig.ret_len)
    {
        *len = 0;
        return I2CState_t::I2C_ERROR;
    }

    return I2CState_t::I2C_IDLE;
}

I2CState_t i2c_algo_config_save()
{
    _algoConfig.start_magic1 = ALGO_CONFIG_MAGIC1;
    _algoConfig.start_magic2 = ALGO_CONFIG_MAGIC2;
    _algoConfig.end_magic1 = ALGO_CONFIG_MAGIC1;
    _algoConfig.end_magic2 = ALGO_CONFIG_MAGIC2;

    if (internal_flash_write(ALGO_CONFIG_ADDR, &_algoConfig, sizeof(_algoConfig)) != 0)
    {
        return I2CState_t::I2C_ERROR;
    }

    return I2CState_t::I2C_IDLE;
}

I2CState_t i2c_algo_config_clear()
{

    if (internal_flash_clear(ALGO_CONFIG_ADDR, sizeof(_algoConfig)) != 0)
    {
        return I2CState_t::I2C_ERROR;
    }

    return I2CState_t::I2C_IDLE;
}

I2CState_t i2c_algo_invoke()
{

    _algoConfig.invoke = CMD_ALGO_INVOKE_START;

    return I2CState_t::I2C_BUSY;
}

size_t algo_object_cnt_get_preview(char *preview, uint16_t max_length)
{
    if (preview == nullptr)
    {
        return 0;
    }

    return 0;
}

size_t algo_object_det_get_preview(char *preview, uint16_t max_length)
{
    if (preview == nullptr)
    {
        return 0;
    }

    return 0;
}

void i2c_algo_task()
{
    if (_algoConfig.invoke == CMD_ALGO_INVOKE_START)
    {
        board_delay_ms(3000);

        _algoConfig.invoke = CMD_ALGO_INVOKE_STOP;
        i2c->set_state(I2CState_t::I2C_IDLE);
        //  _printf("invoke algo done\n");
    }
}

void i2c_task(void *arg)
{
    i2c_algo_task();
}

I2CServer *i2c_server_init()
{

    i2c = I2CServer::get_instance();

    if (i2c == NULL)
    {
        printf("i2c init failed");
        return NULL;
    }

    i2c->register_task(i2c_task);

    /* register i2c event callback */

    // system command
    i2c->register_command(FEATURE_SYSTEM, CMD_SYS_READ_VERSION, CMD_SYS_VERSION_LENGTH, false, nullptr, i2c_read_version, nullptr);
    i2c->register_command(FEATURE_SYSTEM, CMD_SYS_READ_ID, CMD_SYS_ID_LENGTH, false, nullptr, i2c_read_id, nullptr);
    i2c->register_command(FEATURE_SYSTEM, CMD_SYS_READ_STATE, CMD_SYS_STATE_LENGTH, false, nullptr, i2c_read_state, nullptr);
    i2c->register_command(FEATURE_SYSTEM, CMD_SYS_RESET, 0, false, i2c_reset, nullptr, nullptr);

    // algorithm command
    i2c->register_command(FEATURE_ALGO, CMD_ALGO_READ_ALGO, CMD_ALGO_ALGO_LENGTH, false, nullptr, i2c_read_algo, nullptr);
    i2c->register_command(FEATURE_ALGO, CMD_ALGO_WRITE_ALGO, CMD_ALGO_ALGO_LENGTH, true, nullptr, nullptr, i2c_write_algo);
    i2c->register_command(FEATURE_ALGO, CMD_ALGO_READ_MODEL, CMD_ALGO_MODEL_LENGTH, false, nullptr, i2c_read_model, nullptr);
    i2c->register_command(FEATURE_ALGO, CMD_ALGO_WRITE_MODEL, CMD_ALGO_MODEL_LENGTH, true, nullptr, nullptr, i2c_write_model);
    i2c->register_command(FEATURE_ALGO, CMD_ALGO_READ_VALID_MODEL, CMD_ALGO_VALID_MODEL_LENGTH, false, nullptr, i2c_read_valid_model, nullptr);
    i2c->register_command(FEATURE_ALGO, CMD_ALGO_READ_CONFIDENCE, CMD_ALGO_CONFIDENCE_LENGTH, false, nullptr, i2c_read_confidence, nullptr);
    i2c->register_command(FEATURE_ALGO, CMD_ALGO_WRITE_CONFIDENCE, CMD_ALGO_CONFIDENCE_LENGTH, true, nullptr, nullptr, i2c_write_confidence);
    i2c->register_command(FEATURE_ALGO, CMD_ALGO_READ_RET_LEN, CMD_ALGO_READ_RET_LEN_LENGTH, true, nullptr, i2c_read_ret_len, nullptr);
    i2c->register_command(FEATURE_ALGO, CMD_ALGO_READ_RET, 0, true, nullptr, i2c_read_ret, nullptr);
    i2c->register_command(FEATURE_ALGO, CMD_ALGO_CONFIG_SAVE, 0, true, i2c_algo_config_save, nullptr, nullptr);
    i2c->register_command(FEATURE_ALGO, CMD_ALGO_CONFIG_CLEAR, 0, true, i2c_algo_config_clear, nullptr, nullptr);
    i2c->register_command(FEATURE_ALGO, CMD_ALGO_INOVKE, 0, true, i2c_algo_invoke, nullptr, nullptr);

    LOGGER_INFO("i2c server init done\n");

    /* init i2c */
    i2c_algo_init();

    memset(i2c_read_buf, 0x00, sizeof(i2c_read_buf));
    memset(i2c_write_buf, 0x00, sizeof(i2c_write_buf));

    I2CCOMM_CFG_T i2c_cfg;
    i2c_cfg.slv_addr = I2C_SLAVE_ADDR;
    i2c_cfg.read_cb = i2c_event_rx_cb;
    i2c_cfg.write_cb = NULL;           // i2c_event_tx_cb;
    i2c_cfg.err_cb = i2c_event_err_cb; // i2c_event_err_cb;
    hx_lib_i2ccomm_init(i2c_cfg);

    hx_lib_i2ccomm_enable_write(i2c_write_buf);
    hx_lib_i2ccomm_start(i2c_read_buf, I2CCOMM_MAX_RBUF_SIZE);

#ifdef I2C_SYNC_PIN
    /*
     In order to make the E5 cat detect the finish of himax startup.
     we set the GPIO of i2c sync to unavailable first and then
     available.
     */
    hx_drv_iomux_set_pmux(I2C_SYNC_PIN, 3);
    hx_drv_iomux_set_outvalue(I2C_SYNC_PIN, 1 - I2C_SYNC_STATE);
    board_delay_ms(5);
    hx_drv_iomux_set_outvalue(I2C_SYNC_PIN, I2C_SYNC_STATE);
#endif

    return i2c;
}

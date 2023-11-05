/**
*****************************************************************************************
*     Copyright(c) 2022, Seeed Technology Corporation. All rights reserved.
*****************************************************************************************
* @file      cmd_algo_event.h
* @brief     处理算法相关事务事务
* @author    Hongtai Liu (lht856@foxmail.com)
* @date      2022-04-24
* @version   v1.0
**************************************************************************************
* @attention
* <h2><center>&copy; COPYRIGHT 2022 Seeed Technology Corporation</center></h2>
**************************************************************************************
*/

#include <stdint.h>
#include "cmd_def.h"
#include "tflitemicro_algo_common.h"

/* Exported macros ---------------------------------------------------------*/
#define FEATURE_ALGO 0xA0

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
#define CMD_ALGO_READ_POINTER 0x60
#define CMD_ALGO_WRITE_POINTER 0x61
#define CMD_ALGO_POINTER_LENGTH 0x14
#define CMD_ALGO_RANGE_LENGTH 0x08
#define CMD_ALGO_INOVKE 0xA0
#define CMD_ALGO_READ_RET_LEN 0xA1
#define CMD_ALGO_READ_RET_LEN_LENGTH 0x02
#define CMD_ALGO_READ_RET 0xA2
#define CMD_ALGO_CONFIG_SAVE 0xEE
#define CMD_ALGO_CONFIG_CLEAR 0xEF

#define ALGO_CONFIG_MAGIC1 0xc2d25ec8
#define ALGO_CONFIG_MAGIC2 0xfe3cda40

typedef enum
{
    CMD_ALGO_INVOKE_STOP = 0x00,
    CMD_ALGO_INVOKE_START = 0x01
} CMD_ALGO_STATE_T;

/**
 * @brief
 *
 *
 *
 */
typedef struct 
{
    uint32_t start_magic1;
    uint32_t start_magic2;
    uint8_t algo;
    uint8_t model;
    uint8_t state;
    uint8_t rotate;
    int32_t invoke;
    uint32_t period;
    uint8_t confidence;
    uint8_t iou;
    uint16_t ret_len;
    algo_software_config_t config;
    uint8_t reserved2[512 - sizeof(algo_software_config_t) - 31];
    uint32_t end_magic1;
    uint32_t end_magic2;
} cmd_algo_event_t;

extern volatile cmd_algo_event_t g_algo_handler;
extern cmd_event_t algo_event_list[];

ERROR_T cmd_algo_event_init(void);
void cmd_algo_event_task(void *args);
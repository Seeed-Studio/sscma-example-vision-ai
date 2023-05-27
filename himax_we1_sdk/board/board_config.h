/*
* board_config.h
*
*  Created on: 2018/10/25
*      Author: 902452
*/

#ifndef SOCKET_CONFIGS_24_BOARD_CONFIG_H_
#define SOCKET_CONFIGS_24_BOARD_CONFIG_H_

#include "arc_em.h"
#include "arc_builtin.h"

//#include "sysconf.h"
#include "hx_drv_uart.h"
#include "hx_drv_iic_m.h"
#include "hx_drv_iic_s.h"
#include "hx_drv_timer.h"
#include "arc_timer.h"

#ifdef ARC_FEATURE_DMP_PERIPHERAL
#define PERIPHERAL_BASE     ARC_FEATURE_DMP_PERIPHERAL
#else
#define PERIPHERAL_BASE     _arc_aux_read(AUX_DMP_PERIPHERAL)
#endif

/*-----------------------------------------------------------------------------*/

#define BOARD_SYS_TIMER_ID          TIMER_0
#define BOARD_SYS_TIMER_INTNO       INTNO_TIMER0
#define BOARD_SYS_TIMER_HZ          (1000)
#define BOARD_SYS_TIMER_HZ_100MS    (10)
#define BOARD_SYS_TIMER_HZ_1S       (1)

#define BOARD_STD_TIMER_ID          TIMER_1
#define BOARD_STD_TIMER_INTNO       INTNO_TIMER1
#define BOARD_STD_TIMER_HZ          (1000)
#define BOARD_STD_TIMER_HZ_100MS    (10)

#define BOARD_RTC_TIMER_ID          TIMER_RTC
#define BOARD_RTC_TIMER_HZ          (1000)

/** board timer count frequency (HZ) */
#define BOARD_SYS_TIMER_MS_HZ       (1000)
/** board timer count frequency convention based on the global timer counter */
#define BOARD_SYS_TIMER_MS_CONV     (BOARD_CPU_CLOCK/BOARD_SYS_TIMER_HZ)
/** board timer count frequency (HZ) */
#define BOARD_SYS_TIMER_US_HZ       (1000000)
/** board timer count frequency convention based on the global timer counter */
#define BOARD_SYS_TIMER_US_CONV     (BOARD_CPU_CLOCK/BOARD_SYS_TIMER_US_HZ)


#define BOARD_OS_TIMER_ID                   TIMER_0
#define BOARD_OS_TIMER_INTNO                INTNO_TIMER0

#define BOARD_CPU_CLOCK                     (400000000U)

#define OSP_DELAY_OS_COMPAT_ENABLE          (1)
#define OSP_DELAY_OS_COMPAT_DISABLE         (0)

#define OSP_GET_CUR_SYSHZ()                 (gl_sys_hz_cnt)
#define OSP_GET_CUR_MS()                    (gl_sys_ms_cnt)
#define OSP_GET_CUR_US()                    board_get_cur_us()
#define OSP_GET_CUR_HWTICKS()               board_get_hwticks()

/*-----------------------------------------------------------------------------*/

/* common macros must be defined by socket boards */
#define BOARD_TEMP_SENSOR_IIC_ID        USE_SS_IIC_1
#define BOARD_TEMP_IIC_SLVADDR          TEMP_I2C_SLAVE_ADDRESS

#define BOARD_CIS_SENSOR_IIC_ID         USE_SS_IIC_2
#define BOARD_CIS_IIC_SLVADDR           (0x24)

#define BOARD_CONSOLE_DEBUG             1

#define BOARD_CONSOLE_UART_ID           DFSS_UART_1_ID
#define BOARD_COMMAND_UART_ID           DFSS_UART_0_ID

/*TODO: override in project.mk */
#define BOARD_CONSOLE_UART_BAUD         UART_BAUDRATE_921600
#define BOARD_COMMAND_UART_BAUD         UART_BAUDRATE_921600


#define BOARD_DFSS_IICS_ID              (USE_SS_IIC_S_0)
#define BOARD_DFSS_IICS_SLVADDR         (0x62)
#if defined(CIS_OV2640_BAYER) || defined(CIS_OV_SENSOR)
#define SCCB_ID							(0X30)  	//OV2640 I2C ID
#endif


typedef enum {
    BOARD_INIT_SPEC_PERI_I2CM =	0x1<<0,
	BOARD_INIT_SPEC_PERI_I2CS =	0x1<<1,
	BOARD_INIT_SPEC_PERI_UART =	0x1<<2,
	BOARD_INIT_SPEC_PERI_SPIM =	0x1<<3,
	BOARD_INIT_SPEC_PERI_SPIS =	0x1<<4,
	BOARD_INIT_SPEC_PERI_GPIO =	0x1<<5,
	BOARD_INIT_SPEC_PWM = 		0x1<<6,
	BOARD_INIT_SPEC_DP = 		0x1<<7,
	BOARD_INIT_SPEC_AUDIO = 	0x1<<8,
	BOARD_INIT_SPEC_EVT = 		0x1<<9,
	BOARD_INIT_SPEC_SECURITY = 	0x1<<10,
} BOARD_INIT_SPEC_E;


#ifdef __cplusplus
extern "C" {
#endif

extern void specific_board_init(BOARD_INIT_SPEC_E INIT_ITEM);
extern void board_init(void);
extern void board_timer_update(uint32_t precision);
extern void board_delay_ms(uint32_t ms);

#ifdef __cplusplus
}
#endif

#endif /* SOCKET_CONFIGS_24_BOARD_CONFIG_H_ */

#include "sensor/imu/imu_core.h"
#include "sensor/imu/hx_drv_lsm6ds3.h"
#include "sensor/imu/lsm6ds3.h"
#include "hx_drv_iic_m.h"
#include "embARC_debug.h"

#include "tflitemicro_algo.h"
#include <unistd.h>
#define IMU_TEST 1
#define NORMALIZED 0
#define MAXVALUE 2
// #define SS_IIC_0_ID 0 /*!< IIC 0 ID macro : master 0*/

#if IMU_TEST

static float data[DATA_ROWS * DATA_COLS];

int main(void)
{
    DEV_LSM6DS3_PTR lsm6ds3 = hx_drv_lsm6ds3_init((USE_SS_IIC_E)SS_IIC_0_ID);
    Loda *loda = new Loda(0.9);
    // loda->predict();
    hx_drv_lsm6ds3_begin(lsm6ds3);

    int index = 0;

    float interval = 16;

    while (index < DATA_ROWS)
    {
        for (int i = 0; i < WINDOW_SIZE; i++)
        {
            if (hx_drv_lsm6ds3_acc_available(lsm6ds3))
            {

                float acc_x = (float)hx_drv_lsm6ds3_read_acc_x(lsm6ds3);
                float acc_y = (float)hx_drv_lsm6ds3_read_acc_y(lsm6ds3);
                float acc_z = (float)hx_drv_lsm6ds3_read_acc_z(lsm6ds3);
                // float acc_x =(float)hx_drv_lsm6ds3_read_gyro_x(lsm6ds3);
                // float acc_y =(float)hx_drv_lsm6ds3_read_gyro_y(lsm6ds3);
                // float acc_z =(float)hx_drv_lsm6ds3_read_gyro_z(lsm6ds3);

                // data[index * DATA_COLS + i*3] = NORMALIZED == 0 ? acc_x : (acc_x>=0?(acc_x > MAXVALUE ? 1 : acc_x / MAXVALUE): acc_x<-MAXVALUE ?-1:acc_x/MAXVALUE);
                // data[index * DATA_COLS + i*3+1] = NORMALIZED == 0 ? acc_x : (acc_x>=0?(acc_x > MAXVALUE ? 1 : acc_x / MAXVALUE): acc_x<-MAXVALUE ?-1:acc_x/MAXVALUE);
                // data[index * DATA_COLS + i*3+2] = NORMALIZED == 0 ? acc_x : (acc_x>=0?(acc_x > MAXVALUE ? 1 : acc_x / MAXVALUE): acc_x<-MAXVALUE ?-1:acc_x/MAXVALUE);
                data[index * DATA_COLS + i] = NORMALIZED == 0 ? acc_x : (acc_x >= 0 ? (acc_x > MAXVALUE ? 1 : acc_x / MAXVALUE) : acc_x < -MAXVALUE ? -1
                                                                                                                                                    : acc_x / MAXVALUE);
                data[index * DATA_COLS + 1 * WINDOW_SIZE + i] = NORMALIZED == 0 ? acc_y : (acc_y >= 0 ? (acc_y > MAXVALUE ? 1 : acc_y / MAXVALUE) : acc_y < -MAXVALUE ? -1
                                                                                                                                                                      : acc_y / MAXVALUE);
                data[index * DATA_COLS + 2 * WINDOW_SIZE + i] = NORMALIZED == 0 ? acc_z : (acc_z >= 0 ? (acc_z > MAXVALUE ? 1 : acc_z / MAXVALUE) : acc_z < -MAXVALUE ? -1
                                                                                                                                                                      : acc_z / MAXVALUE);

                board_delay_ms(7);
                EMBARC_PRINTF("index:%d x: %d y:%d z:%d\n", index, (int)(acc_x * 1000), (int)(acc_y * 1000), (int)(acc_z * 1000));
            };
        }
        index++;
    };

    float projections[NUM_CUTS * DATA_COLS];
    float histograms[NUM_CUTS * NUM_BINS];
    float limits[NUM_CUTS * (NUM_BINS + 1)];
    int restult;

    loda->fit(data, histograms, limits, projections);
    float test_data[TEST_ROWS * TEST_COLS];

    while (1)
    {
        index = 0;

        while (index < TEST_ROWS)
        {
            for (int i = 0; i < WINDOW_SIZE; i++)
            {
                if (hx_drv_lsm6ds3_acc_available(lsm6ds3))
                {

                    float acc_x = (float)hx_drv_lsm6ds3_read_acc_x(lsm6ds3);
                    float acc_y = (float)hx_drv_lsm6ds3_read_acc_y(lsm6ds3);
                    float acc_z = (float)hx_drv_lsm6ds3_read_acc_z(lsm6ds3);
                    // float acc_x =(float)hx_drv_lsm6ds3_read_gyro_x(lsm6ds3);
                    // float acc_y =(float)hx_drv_lsm6ds3_read_gyro_y(lsm6ds3);
                    // float acc_z =(float)hx_drv_lsm6ds3_read_gyro_z(lsm6ds3);
                    // test_data[index * TEST_COLS + i*3] = NORMALIZED == 0 ? acc_x : (acc_x>=0?(acc_x > MAXVALUE ? 1 : acc_x / MAXVALUE): acc_x<-MAXVALUE ?-1:acc_x/MAXVALUE);
                    // test_data[index * TEST_COLS + i*3+1] = NORMALIZED == 0 ? acc_x : (acc_x>=0?(acc_x > MAXVALUE ? 1 : acc_x / MAXVALUE): acc_x<-MAXVALUE ?-1:acc_x/MAXVALUE);
                    // test_data[index * TEST_COLS + i*3+2] = NORMALIZED == 0 ? acc_x : (acc_x>=0?(acc_x > MAXVALUE ? 1 : acc_x / MAXVALUE): acc_x<-MAXVALUE ?-1:acc_x/MAXVALUE);
                    test_data[index * TEST_COLS + i] = NORMALIZED == 0 ? acc_x : (acc_x >= 0 ? (acc_x > MAXVALUE ? 1 : acc_x / MAXVALUE) : acc_x < -MAXVALUE ? -1
                                                                                                                                                             : acc_x / MAXVALUE);
                    test_data[index * TEST_COLS + 1 * WINDOW_SIZE + i] = NORMALIZED == 0 ? acc_y : (acc_y >= 0 ? (acc_y > MAXVALUE ? 1 : acc_y / MAXVALUE) : acc_x < -MAXVALUE ? -1
                                                                                                                                                                               : acc_y / MAXVALUE);
                    test_data[index * TEST_COLS + 2 * WINDOW_SIZE + i] = NORMALIZED == 0 ? acc_z : (acc_z >= 0 ? (acc_z > MAXVALUE ? 1 : acc_z / MAXVALUE) : acc_x < -MAXVALUE ? -1
                                                                                                                                                                               : acc_z / MAXVALUE);

                    board_delay_ms(8);
                };
            }

            index++;
        };

        restult = loda->predict(test_data, histograms, limits, projections);
        dbg_printf(DBG_LESS_INFO, "RESULTS====>>: %d \n", restult);
    }

    return 0;
}
#endif

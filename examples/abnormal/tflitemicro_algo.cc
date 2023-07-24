#include <stdint.h>
#include <math.h>
#include <stdlib.h>
#include <ctime>
#include "tflitemicro_algo.h"
#include "embARC_debug.h"
#include "hx_drv_timer.h"
#include <stdio.h>
// #include "algo.h"

void print_data(int rows, int cols, const float *data)
{
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            printf("DATA>>%d>>:%f\t", i * cols + j, data[i * cols + j]);
        }
    }
}
void print_data_int(int rows, int cols, const int *data)
{
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            printf("DATAINT_INT>>%d>>:%d\t", i * cols + j, data[i * cols + j]);
        }
    }
}

unsigned long long int rand_int(int rand_seed)
{
    rand_seed = rand_seed * 1103515245 + 12345;
    return rand_seed;
}

void randperm(int n, int index, int *results, int seed)
/*
generate a string of random numbers from 0 to numnber, scramble them,
return the return the previous index number.
*/
{
    int arr[n];
    for (int i = 0; i < n; i++)
    {
        arr[i] = i;
    }
    // 打乱数组
    for (int i = n - 1; i > 0; i--)
    {
        unsigned long long int j = rand_int(board_get_cur_us()) % (i + 1);
        int tmp = arr[i];
        arr[i] = arr[j];
        arr[j] = tmp;
    }

    for (int i = 0; i < index; i++)
    {
        results[i] = arr[i];
    };

    // return result;
};

void dot(float *A, float *B, int rows, int cols, int index, float *results)
/*
Calculate the dot product of matrix A and matrix B.
A is 1D,B is 2D.
dot product resutlt in pointer results.
*/
{

    float tmp = 0;
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            tmp = A[index + j] * B[i * cols + j] + tmp;
        };
        results[i] = tmp;
        tmp = 0;
    };
}

float max_(int len, float *vectors)
/*
calculater a vectors's max number;
*/
{

    if (len > 1)
    {
        float result = -1000;
        for (int i = 0; i < len; i++)
        {
            if (result < vectors[i])
            {
                result = vectors[i];
            };
        };
        return result;
    }
    else if (len == 1)
    {
        return vectors[0];
    }
    else
    {
        return 0.0;
    }
};
float min_(int len, float *vectors)
/*
calculater a vectors's min number;
*/
{
    if (len > 1)
    {
        float result = 1000;
        for (int i = 1; i < len; i++)
        {
            if (result > vectors[i])
            {
                result = vectors[i];
            };
        };
        return result;
    }
    else if (len == 1)
    {
        return vectors[0];
    }
    else
    {
        return 0.0;
    }
};
int getArrayLen(float *data)
{
    return sizeof(data) / sizeof(data[0]);
}

void histogram(int vectors_len, float *vectors, int num_bins, int index_hist, float *hist_int, float *hist_float)
/*
calculater the vectors's histogram,the histogram have num_bins block,
the results save in hist_int with every block number.
the results save in hist_float with every block probability.

*/
{

    float min = min_(vectors_len, vectors);
    float max = max_(vectors_len, vectors);
    float stride = (max - min) / num_bins;
    // int  * hist=new int[num_bins];
    // float stride_ls[num_bins];

    // 直方图两个边界
    hist_float[index_hist * (num_bins + 1)] = min;
    hist_float[index_hist * (num_bins + 1) + num_bins] = max;
    // 直方图esp
    hist_int[0] = 1e-8;
    float sum_ = 1e-8 * num_bins;
    sum_ += vectors_len;

    for (int i = 1; i < num_bins; i++)
    {
        hist_float[index_hist * (num_bins + 1) + i] = hist_float[index_hist * (num_bins + 1) + i - 1] + stride;
        hist_int[index_hist * num_bins + i] = 1e-8;
    };

    for (int i = 0; i < vectors_len; i++)
    {
        for (int j = num_bins - 1; j >= 0; j--)
        {
            if (vectors[i] >= hist_float[index_hist * (num_bins + 1) + j])
            {
                hist_int[index_hist * num_bins + j] += 1;
                break;
            }
        };
    };
    for (int i = 0; i < num_bins; i++)
    {
        hist_int[index_hist * num_bins + i] /= sum_;
    }
}

void randzero_two(int rows, int cols, float *arr)
/*
generate a 2D array,the value of the array all is zero.
the frist dim long of the array is one_dim,
the second dim long of the array is two_dim.
*/
{
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            arr[i * cols + j] = 0.0;
        }
    }
};
void randzero_one(int rows, float *arr)
/*
generate a 1D array,the value of the array all is zero.
the long of the array is one_dim.
*/
{
    for (int i = 0; i < rows; i++)
    {
        arr[i] = 0;
    }
}

float sum_vector(float *vector)
/*
return a vector's sum with float type.
*/
{
    float result = 0;
    int len = sizeof(vector) / sizeof(vector[0]);
    for (int i = 0; i < len; i++)
    {
        result += vector[i];
    }
    return result;
};
int sum_int(int *vector)
/*
return a vector's sum with int type.
*/
{
    int result = 0;
    int len = sizeof(vector) / sizeof(int);
    for (int i = 0; i < len; i++)
    {
        result += vector[i];
    }
    return result;
};

void searchsorted(int sqe_len, const float *sorted_sequence, int input_len, const float *inputs, int sort_index, int *indexs)
{

    for (int i = 0; i < input_len; i++)
    {
        float data = inputs[i];
        for (int j = 0; j < sort_index; j++)
        {
            if (data > sorted_sequence[sqe_len + j] || j == sort_index - 1)
            {
                if (j == sort_index - 1)
                {
                    indexs[i] = j + 1;
                    break;
                }
                continue;
            }
            else if (data <= sorted_sequence[sqe_len + j])
            {
                indexs[i] = j;
                break;
            }
        }
    }
}

float quantile(int len, float *data, float rate)
/*
sorted array data,and return the value of the index that data's lenght mult rate;
*/
{

    int local = (int)(rate * len);
    float tmp;

    for (int i = 0; i < len - 1; i++)
    {
        for (int j = 0; j < len - i - 1; j++)
        {
            if (data[j] > data[j + 1])
            {
                float temp = data[j];
                data[j] = data[j + 1];
                data[j + 1] = temp;
            }
        }
    }

    return data[local];
}

int *fileter(float *data, float threshold)
/*
calculater the every value of the data array wehter more than the thershold.
if more than the thershold mark is one,else is zero.
*/
{
    int len = sizeof(data) / sizeof(data[0]);
    // int results[len];
    int *results = new int[len];
    for (int i = 0; i < len; i++)
    {
        results[i] = data[i] > threshold ? 1 : 0;
    };
    return results;
}

float rand_uniform()
{
    return (float)rand() / RAND_MAX;
}

void randn(int num, float *arr)
{
    float std = 1;
    float mean = 0;
    srand(board_get_cur_us()); // 初始化随机数种子
    for (int i = 0; i < num; i += 2)
    {

        float u1 = (float)rand() / RAND_MAX;
        float u2 = (float)rand() / RAND_MAX;
        float z1 = sqrt(-2 * log(u1)) * cos(2 * M_PI * u2);
        float z2 = sqrt(-2 * log(u1)) * sin(2 * M_PI * u2);
        arr[i] = mean + std * z1;
        if (i + 1 < num)
        {
            arr[i + 1] = mean + std * z2;
        }
    }
}

float fit(float *data, float *projections, float *limits, float *histograms, float yield_rate)
{
    float weights = 1.0 / NUM_CUTS;

    float pred_scores[DATA_ROWS];
    randzero_one(DATA_ROWS, pred_scores);

    float num_features_sqrt = sqrtf(DATA_COLS);
    int num_features_zero = (int)(DATA_COLS - num_features_sqrt) + 1;

    float projections_vectors[DATA_ROWS];
    for (int i = 0; i < NUM_CUTS; i++)
    {
        // generater the number of num_features_zero
        int r[num_features_zero] = {0};
        // the prohections vectors setting with zero
        for (int j = 0; j < num_features_zero; j++)
        {

            projections[i * DATA_COLS + r[j]] = 0.0;
        };
        // calculater projections with data dot product
        dot(projections, data, DATA_ROWS, DATA_COLS, i * DATA_COLS, projections_vectors);
        // calculater histogram of the projections
        histogram(DATA_ROWS, projections_vectors, NUM_BINS, i, histograms, limits);

        int indexs[DATA_ROWS] = {0};
        searchsorted(i * (NUM_BINS + 1), limits, DATA_ROWS, projections_vectors, NUM_BINS - 1, indexs);

        for (int k = 0; k < DATA_ROWS; k++)
        {
            pred_scores[k] += (-weights * log(histograms[i * NUM_BINS + indexs[k]]));
        };
    };

    for (int i = 0; i < DATA_ROWS; i++)
    {
        pred_scores[i] = pred_scores[i] / NUM_CUTS * 1.1;
    }
    float threshold = quantile(DATA_ROWS, pred_scores, yield_rate);

    return threshold;
};
int predict(float *data, float *projections, float *limits, float *histograms, float threshold)
{
    float weights = 1.0 / NUM_CUTS;
    float projections_vectors[DATA_ROWS];
    float pred_scores[TEST_ROWS];
    randzero_one(TEST_ROWS, pred_scores);

    for (int i = 0; i < NUM_CUTS; i++)
    {

        dot(projections, data, TEST_ROWS, TEST_COLS, i * DATA_COLS, projections_vectors);

        int indexs[NUM_BINS] = {0};
        searchsorted(i * (NUM_BINS + 1), limits, TEST_ROWS, projections_vectors, NUM_BINS - 1, indexs);

        for (int j = 0; j < TEST_ROWS; j++)
        {
            pred_scores[j] += -weights * log(histograms[i * NUM_BINS + indexs[j]]) / NUM_CUTS;
        };
    };

    int *pred_ = fileter(pred_scores, threshold);

    return pred_scores[0] > threshold ? 1 : 0;
}

Loda::Loda(float yield_rate)
{
    this->weights = 1.0 / num_cuts;
    this->threshold = 0.0;
};

int Loda::fit(float *data, float *histograms, float *limits, float *projections)
{

    randzero_two(NUM_CUTS, NUM_BINS, histograms);
    randzero_two(NUM_CUTS, NUM_BINS + 1, limits);
    int num_features = DATA_COLS;

    float pred_scores[DATA_ROWS] = {0};
    float num_features_sqrt = sqrtf(num_features);
    int num_features_zero = (int)(num_features - num_features_sqrt) + 1;

    randn(NUM_CUTS * num_features, projections);

    float projections_vectors[DATA_ROWS] = {0};
    int indexs[DATA_ROWS] = {0};
    for (int i = 0; i < NUM_CUTS; i++)
    {
        // generater the number of num_features_zero
        int r[num_features_zero] = {0};
        randperm(num_features, num_features_zero, r, i);
        // the prohections vectors setting with zero
        for (int j = 0; j < num_features_zero; j++)
        {
            projections[i * num_features + r[j]] = 0.0;
        };
        // calculater projections with data dot product
        dot(projections, data, DATA_ROWS, DATA_COLS, i * num_features, projections_vectors);
        // calculater histogram of the projections
        histogram(DATA_ROWS, projections_vectors, NUM_BINS, i, histograms, limits);
        searchsorted(i * (NUM_BINS + 1), limits, DATA_ROWS, projections_vectors, NUM_BINS - 1, indexs);
        for (int k = 0; k < DATA_ROWS; k++)
        {
            pred_scores[k] += (-this->weights * log(histograms[i * NUM_BINS + indexs[k]]));
        };
    };

    for (int i = 0; i < DATA_ROWS; i++)
    {
        pred_scores[i] = pred_scores[i] * 1.06 / NUM_CUTS;
    }
    this->threshold = quantile(DATA_ROWS, pred_scores, this->yield_rate);
    dbg_printf(DBG_LESS_INFO, "restult>>|||: %d \n", (int)(this->threshold * 1000));
    return 1;
};

int Loda::predict(float *data, float *histograms, float *limits, float *projections)
{
    float pred_scores[TEST_ROWS];
    float projections_vectors[DATA_ROWS];

    randzero_one(TEST_ROWS, pred_scores);

    for (int i = 0; i < NUM_CUTS; i++)
    {
        dot(projections, data, TEST_ROWS, TEST_COLS, i * DATA_COLS, projections_vectors);

        int indexs[NUM_BINS];

        searchsorted(i * (NUM_BINS + 1), limits, TEST_ROWS, projections_vectors, NUM_BINS - 1, indexs);

        for (int j = 0; j < TEST_ROWS; j++)
        {
            pred_scores[j] += -weights * log(histograms[i * NUM_BINS + indexs[j]]) / NUM_CUTS;
        };
    };
    int *pred_ = fileter(pred_scores, threshold);

    return pred_scores[0] > threshold ? 1 : 0;
};

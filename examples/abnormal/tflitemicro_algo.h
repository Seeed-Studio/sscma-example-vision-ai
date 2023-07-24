#ifndef ALGO_AB_H
#define ALGO_AB_H

#ifdef __cplusplus
extern "C"
{
#include <stdint.h>
#include <math.h>

#endif
#define DATA_ROWS 100
#define DATA_COLS 90
#define TEST_ROWS 1
#define TEST_COLS 90
#define NUM_CUTS 30
#define NUM_BINS 10
#define WINDOW_SIZE 30
    int loda_algo_run(uint16_t data);
    int loda_algo_predict(uint16_t &data);

    class Loda
    {
    public:
        Loda(float yield_rate);
        int num_bins;
        int num_cuts;
        float yield_rate;
        float weights;
        float *projections;
        float *histograms;
        float *limits;
        float threshold;
        float_t weights_;
        int fit(float_t *data, float *histograms, float *limits, float *projections);
        int predict(float_t *data, float *histograms, float *limits, float *projections);
    };
#ifdef __cplusplus
}

#endif
#endif
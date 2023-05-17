#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <forward_list>
#include <math.h>
#include "logger.h"
#include "algo_digital_meter_nms.h"

#define CLIP(x, y, z) (x < y) ? y : ((x > z) ? z : x)

static bool _object_count_comparator_reverse(object_t &oa, object_t &ob)
{
    return oa.confidence < ob.confidence;
}

static bool _object_nms_comparator(object_t &oa, object_t &ob)
{
    return oa.confidence > ob.confidence;
}


static bool _object_count_comparator(object_t &oa, object_t &ob)
{
    return oa.x < ob.x;
}

bool _object_count_remove(object_t &obj)
{
    return (obj.confidence == 0);
}

static uint16_t _overlap(float x1, float w1, float x2, float w2)
{
    uint16_t l1 = x1 - w1 / 2;
    uint16_t l2 = x2 - w2 / 2;
    uint16_t left = l1 > l2 ? l1 : l2;
    uint16_t r1 = x1 + w1 / 2;
    uint16_t r2 = x2 + w2 / 2;
    uint16_t right = r1 < r2 ? r1 : r2;
    return right - left;
}

void _hard_nms_obeject_count(std::forward_list<object_t> &object_count_list, uint8_t nms)
{
    object_t max_box;
    std::forward_list<object_t>::iterator max_box_obj;
    object_count_list.sort(_object_nms_comparator);
    for (std::forward_list<object_t>::iterator it = object_count_list.begin(); it != object_count_list.end(); ++it)
    {
        uint16_t area = it->w * it->h;
        for (std::forward_list<object_t>::iterator itc = std::next(it, 1); itc != object_count_list.end(); ++itc)
        {
            if (itc->confidence == 0)
            {
                continue;
            }
            uint16_t iw = _overlap(itc->x, itc->w, it->x, it->w);
            if (iw > 0)
            {
                uint16_t ih = _overlap(itc->y, itc->h, it->y, it->h);
                if (ih > 0)
                {
                    float ua = float(itc->w * itc->h + area - iw * ih);
                    float ov = iw * ih / ua;
                    if (int(float(ov) * 100) >= nms)
                    {
                        itc->confidence = 0;
                    }
                }
            }
        }
    }
    object_count_list.remove_if(_object_count_remove);

    return;
}

std::forward_list<object_t> nms_get_obeject_topn(int8_t *dataset, uint16_t top_n, uint8_t threshold, uint8_t nms, uint16_t width, uint16_t height, int num_record, int8_t num_class, float scale, int zero_point)
{
    std::forward_list<object_t> object_count_list[num_class];
    int16_t num_obj[num_class] = {0};
    int16_t num_element = num_class + OBJECT_T_INDEX;
    for (int i = 0; i < num_record; i++)
    {
        float confidence = float(dataset[i * num_element + OBJECT_C_INDEX] - zero_point) * scale;

        if (int(float(confidence) * 100) >= threshold)
        {
            object_t obj;
            int8_t max = -128;
            obj.target = 0;
            for (int j = 0; j < num_class; j++)
            {
                if (max < dataset[i * num_element + OBJECT_T_INDEX + j])
                {
                    max = dataset[i * num_element + OBJECT_T_INDEX + j];
                    obj.target = j;
                }
            }

            int x = int(float(float(dataset[i * num_element + OBJECT_X_INDEX] - zero_point) * scale) * width);
            int y = int(float(float(dataset[i * num_element + OBJECT_Y_INDEX] - zero_point) * scale) * height);
            int w = int(float(float(dataset[i * num_element + OBJECT_W_INDEX] - zero_point) * scale) * width);
            int h = int(float(float(dataset[i * num_element + OBJECT_H_INDEX] - zero_point) * scale) * height);

            obj.x = CLIP(x, 0, width);
            obj.y = CLIP(y, 0, height);
            obj.w = CLIP(w, 0, width);
            obj.h = CLIP(h, 0, height);
            obj.confidence = int(float(confidence) * 100);
            if (num_obj[obj.target] >= top_n)
            {
                object_count_list[obj.target].sort(_object_count_comparator_reverse);
                if (obj.confidence > object_count_list[obj.target].front().confidence)
                {
                    object_count_list[obj.target].pop_front();
                    object_count_list[obj.target].emplace_front(obj);
                }
            }
            else
            {
                object_count_list[obj.target].emplace_front(obj);
                num_obj[obj.target]++;
            }
        }
    }

    std::forward_list<object_t> result;

    for (int i = 0; i < num_class; i++)
    {
        result.splice_after(result.before_begin(), object_count_list[i]);
    }

    _hard_nms_obeject_count(result, nms);

    result.sort(_object_count_comparator); // left to right

    return result;
}

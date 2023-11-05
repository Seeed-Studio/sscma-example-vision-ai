#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <forward_list>
#include <math.h>
#include "grove_ai_config.h"
#include "logger.h"
#include "algo_object_detection.h"
#include "algo_object_detection_nms.h"

#define CLIP(x, y, z) (x < y) ? y : ((x > z) ? z : x)

static bool _object_detection_comparator_reverse(object_detection_t &oa, object_detection_t &ob)
{
    return oa.confidence < ob.confidence;
}

static bool _object_detection_comparator(object_detection_t &oa, object_detection_t &ob)
{
    return oa.confidence > ob.confidence;
}

bool _object_detection_remove(object_detection_t &obj)
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

void _soft_nms_obeject_detection(std::forward_list<object_detection_t> &object_detection_list, uint8_t nms)
{
    object_detection_t max_box;
    std::forward_list<object_detection_t>::iterator max_box_obj;
    object_detection_list.sort(_object_detection_comparator);
    for (std::forward_list<object_detection_t>::iterator it = object_detection_list.begin(); it != object_detection_list.end(); ++it)
    {
        uint16_t area = it->w * it->h;
        for (std::forward_list<object_detection_t>::iterator itc = std::next(it, 1); itc != object_detection_list.end(); ++itc)
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
    object_detection_list.remove_if(_object_detection_remove);

    return;
}

std::forward_list<object_detection_t> nms_get_obeject_detection_topn(int8_t *dataset, uint16_t top_n, uint8_t threshold, uint8_t nms, uint16_t width, uint16_t height, int num_record, int8_t num_class, float scale, int zero_point)
{
    bool rescale = scale < 0.1 ? true : false; // scale < 0.1 means the input is float
    std::forward_list<object_detection_t> object_detection_list[num_class];
    int16_t num_obj[num_class] = {0};
    int16_t num_element = num_class + OBJECT_DETECTION_T_INDEX;
    for (int i = 0; i < num_record; i++)
    {
        float confidence = float(dataset[i * num_element + OBJECT_DETECTION_C_INDEX] - zero_point) * scale;
        confidence = rescale ? confidence * 100 : confidence;

        if (int(confidence) >= threshold)
        {
            object_detection_t obj;
            int8_t max = -128;
            obj.target = 0;
            for (int j = 0; j < num_class; j++)
            {
                if (max < dataset[i * num_element + OBJECT_DETECTION_T_INDEX + j])
                {
                    max = dataset[i * num_element + OBJECT_DETECTION_T_INDEX + j];
                    obj.target = j;
                }
            }

            float x = float(dataset[i * num_element + OBJECT_DETECTION_X_INDEX] - zero_point) * scale;
            float y = float(dataset[i * num_element + OBJECT_DETECTION_Y_INDEX] - zero_point) * scale;
            float w = float(dataset[i * num_element + OBJECT_DETECTION_W_INDEX] - zero_point) * scale;
            float h = float(dataset[i * num_element + OBJECT_DETECTION_H_INDEX] - zero_point) * scale;

            if (rescale)
            {
                obj.x = CLIP(int(x * width), 0, width);
                obj.y = CLIP(int(y * height), 0, height);
                obj.w = CLIP(int(w * width), 0, width);
                obj.h = CLIP(int(h * height), 0, height);
            }
            else
            {
                obj.x = CLIP(int(x), 0, width);
                obj.y = CLIP(int(y), 0, height);
                obj.w = CLIP(int(w), 0, width);
                obj.h = CLIP(int(h), 0, height);
            }

            obj.confidence = int(confidence);

            if (num_obj[obj.target] >= top_n)
            {
                object_detection_list[obj.target].sort(_object_detection_comparator_reverse);
                if (obj.confidence > object_detection_list[obj.target].front().confidence)
                {
                    object_detection_list[obj.target].pop_front();
                    object_detection_list[obj.target].emplace_front(obj);
                }
            }
            else
            {
                object_detection_list[obj.target].emplace_front(obj);
                num_obj[obj.target]++;
            }
        }
    }

    std::forward_list<object_detection_t> result;

    for (int i = 0; i < num_class; i++)
    {
        if (!object_detection_list[i].empty())
        {
            _soft_nms_obeject_detection(object_detection_list[i], nms);
            result.splice_after(result.before_begin(), object_detection_list[i]);
        }
    }

    result.sort(_object_detection_comparator);

    return result;
}

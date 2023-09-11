#pragma once

#include <cstdint>
#include <forward_list>
#include <iomanip>
#include <sstream>
#include <string>
#include <type_traits>
#include <utility>

#include "at_definations.hpp"
#include "el_algorithm.hpp"
//#include "el_base64.h"
#include "el_cv.h"
#include "el_data.hpp"
#include "el_device.h"
#include "el_repl.hpp"
#include "el_types.h"

std::string string_2_str(const std::string& str) {
    std::string ss(1, '"');
    for (char c : str) {
        if (c == '"') [[unlikely]]
            ss += "\\\"";
        else if (c == '\n') [[unlikely]]
            ss += "\\n";
        else if (std::isprint(c)) [[likely]]
            ss += c;
    }
    ss += '"';
    return ss;
}

const char* err_code_2_str(el_err_code_t ec) {
    switch (ec) {
    case EL_OK:
        return "\"ok\"";
    case EL_AGAIN:
        return "\"try again\"";
    case EL_ELOG:
        return "\"logic error\"";
    case EL_ETIMOUT:
        return "\"timeout\"";
    case EL_EIO:
        return "\"input/output error\"";
    case EL_EINVAL:
        return "\"invalid argument\"";
    case EL_ENOMEM:
        return "\"out of memory\"";
    case EL_EBUSY:
        return "\"busy\"";
    case EL_ENOTSUP:
        return "\"not supported\"";
    case EL_EPERM:
        return "\"operation not permitted\"";
    default:
        return "\"unknown\"";
    }
}

const char* algo_type_2_str(el_algorithm_type_t at) {
    switch (at) {
    case EL_ALGO_TYPE_FOMO:
        return "\"FOMO\"";
    case EL_ALGO_TYPE_PFLD:
        return "\"PFLD\"";
    case EL_ALGO_TYPE_YOLO:
        return "\"YOLO\"";
    case EL_ALGO_TYPE_IMCLS:
        return "\"IMCLS\"";
    default:
        return "\"undefined\"";
    }
}

const char* algo_cat_2_str(el_algorithm_cat_t ac) {
    switch (ac) {
    case EL_ALGO_CAT_DET:
        return "\"detection\"";
    case EL_ALGO_CAT_POSE:
        return "\"pose\"";
    case EL_ALGO_CAT_CLS:
        return "\"classification\"";
    default:
        return "\"undefined\"";
    }
}

const char* sensor_type_2_str(el_sensor_type_t st) {
    switch (st) {
    case EL_SENSOR_TYPE_CAM:
        return "\"camera\"";
    default:
        return "\"undefined\"";
    }
}

const char* sensor_sta_2_str(el_sensor_state_t ss) {
    switch (ss) {
    case EL_SENSOR_STA_REG:
        return "\"registered\"";
    case EL_SENSOR_STA_AVAIL:
        return "\"available\"";
    case EL_SENSOR_STA_LOCKED:
        return "\"locked\"";
    default:
        return "\"unknown\"";
    }
}

const char* img_type_2_str(el_pixel_format_t pix_fmt) {
    switch (pix_fmt) {
    case EL_PIXEL_FORMAT_GRAYSCALE:
        return "\"grayscale\"";
    case EL_PIXEL_FORMAT_JPEG:
        return "\"jpeg\"";
    case EL_PIXEL_FORMAT_RGB565:
        return "\"rgb565\"";
    case EL_PIXEL_FORMAT_RGB888:
        return "\"rgb888\"";
    case EL_PIXEL_FORMAT_YUV422:
        return "\"yuv422\"";
    default:
        return "\"undefined\"";
    }
}

inline uint32_t color_literal(uint8_t i) {
    static const uint16_t color[] = {
      0x0000,
      0x03E0,
      0x001F,
      0x7FE0,
      0xFFFF,
    };
    return color[i % 5];
}

void draw_results_on_image(const std::forward_list<el_class_t>& results, el_img_t* img) {}

void draw_results_on_image(const std::forward_list<el_point_t>& results, el_img_t* img) {
    uint8_t i = 0;
    for (const auto& point : results) edgelab::el_draw_point(img, point.x, point.y, color_literal(++i));
}

void draw_results_on_image(const std::forward_list<el_box_t>& results, el_img_t* img) {
    uint8_t i = 0;
    for (const auto& box : results) {
        int16_t y = box.y - box.h / 2;
        int16_t x = box.x - box.w / 2;
        edgelab::el_draw_rect(img, x, y, box.w, box.h, color_literal(++i), 4);
    }
}

std::string model_info_2_json(el_model_info_t model_info) {
    char buffer[512] = {0};
    snprintf(buffer, sizeof(buffer), "{\"id\": %d, \"type\": %d, \"address\": \"0x%x\", \"size\": \"0x%x\"}", 
        static_cast<unsigned>(model_info.id), static_cast<unsigned>(model_info.type), static_cast<unsigned>(model_info.addr_flash), static_cast<unsigned>(model_info.size));
    
    std::string str(buffer);
    return str;
}

std::string sensor_info_2_json(el_sensor_info_t sensor_info) {
    char buffer[512] = {0};

    snprintf(buffer, sizeof(buffer), "{\"id\": %d, \"type\": %d, \"state\": %d}", 
        static_cast<unsigned>(sensor_info.id), static_cast<unsigned>(sensor_info.type), static_cast<unsigned>(sensor_info.state));
    
    std::string str(buffer);
    return str;
}

template <typename T> constexpr std::string results_2_json(const std::forward_list<T>& results) {
    char buffer[512] = {0};
    int length = 0;
    using F                = std::function<void(void)>;
    F delim_f       = []() {};
    F print_delim_f = [&buffer,&length]() { length += snprintf(buffer, sizeof(buffer), ", "); };
    F print_void_f  = [&]() { delim_f = print_delim_f; };
    delim_f                = print_void_f;
    if constexpr (std::is_same<T, el_box_t>::value) {
        length += snprintf(buffer + length, sizeof(buffer) - length, "\"boxes\": [");
        for (const auto& box : results) {
            delim_f();
            length += snprintf(buffer + length, sizeof(buffer) - length, "[%d, %d, %d, %d, %d, %d]", 
                static_cast<unsigned>(box.x), static_cast<unsigned>(box.y),
                static_cast<unsigned>(box.w), static_cast<unsigned>(box.h),
                static_cast<unsigned>(box.target), static_cast<unsigned>(box.score));
        }
    } else if constexpr (std::is_same<T, el_point_t>::value) {
        length += snprintf(buffer + length, sizeof(buffer) - length, "\"points\": [");
        for (const auto& point : results) {
            delim_f();
            length += snprintf(buffer + length, sizeof(buffer) - length, "[%d, %d, %d]", 
                static_cast<unsigned>(point.x), static_cast<unsigned>(point.y), 
                static_cast<unsigned>(point.target));
        }
    } else if constexpr (std::is_same<T, el_class_t>::value) {
        length += snprintf(buffer + length, sizeof(buffer) - length, "\"classes\": [");
        for (const auto& cls : results) {
            delim_f();
            length += snprintf(buffer + length, sizeof(buffer) - length, "[%d, %d]", 
                static_cast<unsigned>(cls.score), static_cast<unsigned>(cls.target));
        }
    }
    snprintf(buffer + length, sizeof(buffer) - length, "]");
    std::string str(buffer);

    return str;
}

// TODO: avoid repeatly allocate/release memory in for loop
std::string img_2_json_str(const el_img_t* img) {
    using namespace edgelab;
    char buffer[512] = {0};
    int length = 0;

    if (!img || !img->data) [[unlikely]]
        return {};

    length += snprintf(buffer, sizeof(buffer), "\"jpeg\": \"");
    size_t size     = img->width * img->height * 3;
    auto   jpeg_img = el_img_t{.data   = new uint8_t[size]{},
                               .size   = size,
                               .width  = img->width,
                               .height = img->height,
                               .format = EL_PIXEL_FORMAT_JPEG,
                               .rotate = img->rotate};

    el_err_code_t ret = EL_OK;
#ifdef CONFIG_EL_LIB_JPEGENC
    ret = rgb_to_jpeg(img, &jpeg_img);
#endif
    if (ret == EL_OK) [[likely]] {
        auto* buffer = new char[((jpeg_img.size + 2) / 3) * 4 + 1]{};
        //TODO: need fix
        //el_base64_encode(jpeg_img.data, jpeg_img.size, buffer);
        length += snprintf(buffer + length, sizeof(buffer) - length, "%s", buffer);
        delete[] buffer;
    }
    delete[] jpeg_img.data;
    length += snprintf(buffer + length, sizeof(buffer) - length, "\"");
    std::string str(buffer);

    return str;
}

std::string simple_reply_ok(const std::string& cmd) {
    std::string str{};
    str += "{\"";
    str += cmd;
    str += "\": {\"status\": \"OK\"}}";
    return str;
}

template <typename ConfigType> std::string algorithm_config_2_json_str(const ConfigType& config) {
    using namespace edgelab;
    char buffer[512] = {0};
    int length = 0;

    if constexpr (std::is_same<ConfigType, el_algorithm_fomo_config_t>::value ||
                  std::is_same<ConfigType, el_algorithm_imcls_config_t>::value)
        length += snprintf(buffer, sizeof(buffer), "\"score_threshold\": %d", 
            static_cast<unsigned>(config.score_threshold));
    else if constexpr (std::is_same<ConfigType, el_algorithm_yolo_config_t>::value) {
        length += snprintf(buffer + length, sizeof(buffer) - length, "\"score_threshold\": %d", 
            static_cast<unsigned>(config.score_threshold));
        length += snprintf(buffer + length, sizeof(buffer) - length, ", \"iou_threshold\": ", 
            static_cast<unsigned>(config.iou_threshold));
    }

    std::string str(buffer);
    return str;
}

template <typename AlgorithmType>
std::string img_invoke_results_2_json_str(
  const AlgorithmType* algorithm, const el_img_t* img, const std::string& cmd, bool result_only, el_err_code_t ret) {
    using namespace edgelab;
    char buffer[512] = {0};
    int length = 0;

    length += snprintf(buffer, sizeof(buffer), "%s\"name\": \"%s\", \"code\": %d", 
        REPLY_EVT_HEADER, cmd.c_str(), static_cast<int>(ret));
    length += snprintf(buffer + length, sizeof(buffer) - length, ", \"data\": {\"perf\": [%d,%d,%d], %s", 
        static_cast<unsigned>(algorithm->get_preprocess_time()),
        static_cast<unsigned>(algorithm->get_run_time()), 
        static_cast<unsigned>(algorithm->get_postprocess_time()),
        results_2_json(algorithm->get_results()));
    if (!result_only)
        length += snprintf(buffer + length, sizeof(buffer) - length, ", %s", 
            img_2_json_str(img).c_str());
    length += snprintf(buffer + length, sizeof(buffer) - length, "}}\n");

    std::string str(buffer);
    return str;
}

template <typename AlgorithmType> class AlgorithmConfigHelper {
   public:
    using ConfigType = typename AlgorithmType::ConfigType;

    AlgorithmConfigHelper(AlgorithmType* algorithm)
        : _instance(edgelab::ReplDelegate::get_delegate()->get_server_handler()),
          _algorithm(algorithm),
          _config(algorithm->get_algorithm_config()),
#ifdef CONFIG_EL_LIB_FLASHDB
          _kv(el_make_storage_kv_from_type(_config)),
          //_storage(edgelab::DataDelegate::get_delegate()->get_storage_handler()),
#endif
          _serial(edgelab::Device::get_device()->get_serial()) 
          {
        using namespace edgelab;
        
#ifdef CONFIG_EL_LIB_FLASHDB
        if (_storage->contains(_kv.key)) [[likely]]
            *_storage >> _kv;
        else
            *_storage << _kv;
        _algorithm->set_algorithm_config(_kv.value);
#endif
        if constexpr (std::is_same<ConfigType, el_algorithm_fomo_config_t>::value ||
                      std::is_same<ConfigType, el_algorithm_imcls_config_t>::value ||
                      std::is_same<ConfigType, el_algorithm_yolo_config_t>::value) {
            el_err_code_t ret = _instance->register_cmd(
              "TSCORE", "Set score threshold", "SCORE_THRESHOLD", [this](std::vector<std::string> argv) {
                  char buffer[512] = {0};
                  uint8_t       value = std::atoi(argv[1].c_str());
                  el_err_code_t ret   = value <= 100 ? EL_OK : EL_EINVAL;

                  if (ret == EL_OK) {
                      this->_algorithm->set_score_threshold(value);
#ifdef CONFIG_EL_LIB_FLASHDB
                      this->_kv.value.score_threshold = value;
                      *(this->_storage) << this->_kv;
#endif
                  }
                  memset(buffer, 0x00, sizeof(buffer));
                  snprintf(buffer, sizeof(buffer), "%s\"name\": \"%s\", \"code\": %d, \"data\": \"%d\"}\n", 
                    REPLY_CMD_HEADER, argv[0].c_str(), static_cast<int>(ret), 
#ifdef CONFIG_EL_LIB_FLASHDB
                    static_cast<unsigned>(this->_kv.value.score_threshold)
#else
                    EL_OK
#endif
                    );

                  this->_serial->send_bytes(buffer, strlen(buffer));

                  return EL_OK;
              });
            if (ret == EL_OK) _config_cmds.emplace_front("TSCORE");

            ret = _instance->register_cmd("TSCORE?", "Get score threshold", "", [this](std::vector<std::string> argv) {
                char buffer[512] = {0};

                snprintf(buffer, sizeof(buffer), "%s\"name\": \"%s\", \"code\": %d, \"data\": \"%d\"}\n", 
                    REPLY_CMD_HEADER, argv[0].c_str(), static_cast<int>(EL_OK), 
                    static_cast<unsigned>(this->_algorithm->get_score_threshold()));

                this->_serial->send_bytes(buffer, strlen(buffer));

                return EL_OK;
            });
            if (ret == EL_OK) _config_cmds.emplace_front("TSCORE?");
        }
        if constexpr (std::is_same<ConfigType, el_algorithm_yolo_config_t>::value) {
            el_err_code_t ret = _instance->register_cmd(
              "TIOU", "Set IoU threshold", "IOU_THRESHOLD", [this](std::vector<std::string> argv) {
                  char buffer[512] = {0};
                  uint8_t       value = std::atoi(argv[1].c_str());
                  el_err_code_t ret   = value <= 100 ? EL_OK : EL_EINVAL;

                  if (ret == EL_OK) {
                      this->_algorithm->set_iou_threshold(value);
#ifdef CONFIG_EL_LIB_FLASHDB
                      this->_kv.value.iou_threshold = value;
                      *(this->_storage) << this->_kv;
#endif
                  }

                  snprintf(buffer, sizeof(buffer), "%s\"name\": \"%s\", \"code\": %d, \"data\": \"%d\"}\n", 
                    REPLY_CMD_HEADER, argv[0].c_str(), static_cast<int>(ret),
#ifdef CONFIG_EL_LIB_FLASHDB
                    static_cast<unsigned>(this->_kv.value.iou_threshold)
#else
                    EL_OK
#endif
                    );

                  this->_serial->send_bytes(buffer, strlen(buffer));

                  return EL_OK;
              });
            if (ret == EL_OK) _config_cmds.emplace_front("TIOU");

            ret = _instance->register_cmd("TIOU?", "Get IoU threshold", "", [this](std::vector<std::string> argv) {
                char buffer[512] = {0};

                snprintf(buffer, sizeof(buffer), "%s\"name\": \"%s\", \"code\": %d, \"data\": \"%d\"}\n", 
                    REPLY_CMD_HEADER, argv[0].c_str(), static_cast<int>(EL_OK), 
                    static_cast<unsigned>(this->_algorithm->get_iou_threshold()));

                this->_serial->send_bytes(buffer, strlen(buffer));

                return EL_OK;
            });
            if (ret == EL_OK) _config_cmds.emplace_front("TIOU?");
        }
    }

    ConfigType dump_config() { return _config; }

    ~AlgorithmConfigHelper() {
        for (const auto& cmd : _config_cmds) _instance->unregister_cmd(cmd);
    }

   private:
    edgelab::repl::ReplServer*     _instance;
    std::forward_list<std::string> _config_cmds;

    AlgorithmType*                                     _algorithm;
    ConfigType                                         _config;
#ifdef CONFIG_EL_LIB_FLASHDB
    edgelab::data::types::el_storage_kv_t<ConfigType&> _kv;

    edgelab::data::Storage* _storage;
#endif
    edgelab::Serial*        _serial;
};

std::vector<std::string> tokenize_function_2_argv(const std::string& input) {
    std::vector<std::string> argv;

    size_t index = 0;
    size_t size  = input.size();
    char   c     = {};

    while (index < size) {
        c = input.at(index);
        if (std::isalnum(c) || c == '_') {
            size_t prev = index;
            while (++index < size) {
                c = input.at(index);
                if (!std::isalnum(c) && c != '_') [[unlikely]]
                    break;
            }
            argv.push_back(input.substr(prev, index - prev));
        } else
            ++index;
    }
    argv.shrink_to_fit();

    return argv;
}

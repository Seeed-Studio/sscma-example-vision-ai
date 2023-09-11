#pragma once

#include <algorithm>
#include <atomic>
#include <cstdint>
#include <cstring>
#include <forward_list>
#include <functional>
#include <iomanip>
#include <memory>
#include <sstream>
#include <string>

#include "at_action.hpp"
#include "at_definations.hpp"
#include "at_utility.hpp"
#include "edgelab.h"

void at_server_echo_cb(el_err_code_t ret, const std::string& msg) {
    auto* serial = Device::get_device()->get_serial();
    char buffer[512] = {0};

    if (ret != EL_OK)
        snprintf(buffer, sizeof(buffer), "%s\"name\": \"AT\", \"code\": %d, \"data\": %s}\n", 
            REPLY_LOG_HEADER, (int)ret, msg.c_str());

    std::string str(buffer);
    serial->send_bytes(str.c_str(), str.size());
}

void at_print_help(std::forward_list<el_repl_cmd_t> cmd_list) {
    auto* serial = Device::get_device()->get_serial();
    char buffer[4096] = {0};
    int length = 0;

    for (const auto& cmd : cmd_list) {
        length += snprintf(buffer + length, sizeof(buffer) - length, "  AT+%s", cmd.cmd.c_str());
        if (cmd.args.size()) {
            length += snprintf(buffer + length, sizeof(buffer) - length, "=<%s>", cmd.args.c_str());
        }
        length += snprintf(buffer + length, sizeof(buffer) - length, "\n    %s\n", cmd.desc.c_str());
    }

    std::string str(buffer);
    serial->send_bytes(str.c_str(), str.size());
}

void at_get_device_id(const std::string& cmd) {
    auto* device = Device::get_device();
    auto* serial = device->get_serial();
    char buffer[512] = {0};

    snprintf(buffer, sizeof(buffer), "%s\"name\": \"%s\", \"code\": %d, \"data\": \"0x%x\"}\n", 
        REPLY_CMD_HEADER, cmd.c_str(), static_cast<int>(EL_OK), device->get_device_id());

    std::string str(buffer);
    serial->send_bytes(str.c_str(), str.size());
}

void at_get_device_name(const std::string& cmd) {
    auto* device = Device::get_device();
    auto* serial = device->get_serial();
    char buffer[512] = {0};

    snprintf(buffer, sizeof(buffer), "%s\"name\": \"%s\", \"code\": %d, \"data\": \"%s\"}\n",
        REPLY_CMD_HEADER, cmd.c_str(), static_cast<int>(EL_OK), device->get_device_name());

    std::string str(buffer);
    serial->send_bytes(str.c_str(), str.size());
}

void at_get_device_status(const std::string& cmd,
                          int32_t            boot_count,
                          uint8_t            current_model_id,
                          uint8_t            current_sensor_id) {
    auto* device = Device::get_device();
    auto* serial = device->get_serial();
    auto* models = DataDelegate::get_delegate()->get_models_handler();
    char buffer[512] = {0};

    auto model_info  = models->get_model_info(current_model_id);
    auto sensor_info = device->get_sensor_info(current_sensor_id);

    snprintf(buffer, sizeof(buffer), "%s\"name\": \"%s\", \"code\": , \"data\": {\"boot_count\": %d, \"model\": %s, \"sensor\": %s}}\n",
        REPLY_CMD_HEADER, cmd.c_str(), static_cast<int>(EL_OK), static_cast<unsigned>(boot_count), model_info_2_json(model_info), sensor_info_2_json(sensor_info));

    std::string str(buffer);
    serial->send_bytes(str.c_str(), str.size());
}

void at_get_version(const std::string& cmd) {
    auto* device = Device::get_device();
    auto* serial = device->get_serial();
    char buffer[512] = {0};

    snprintf(buffer, sizeof(buffer), "%s\"name\": \"%s\", \"code\": %d, \"data\": {\"software\": \"%s\", \"hardware\": \"%d\"}}\n",
        REPLY_CMD_HEADER, cmd.c_str(), static_cast<int>(EL_OK), EL_VERSION, static_cast<unsigned>(device->get_chip_revision_id()));

    std::string str(buffer);
    serial->send_bytes(str.c_str(), str.size());
}

void at_get_available_algorithms(const std::string& cmd) {
    auto* serial                = Device::get_device()->get_serial();
    auto* algorithm_delegate    = AlgorithmDelegate::get_delegate();
    auto& registered_algorithms = algorithm_delegate->get_all_algorithm_info();
    char buffer[512] = {0};
    int length = 0;

    length += snprintf(buffer, sizeof(buffer), "%s\"name\": \"%s\", \"code\": %d, \"data\": [", REPLY_CMD_HEADER, cmd.c_str(), static_cast<int>(EL_OK));
    for (const auto& i : registered_algorithms) {
        length += snprintf(buffer+length, sizeof(buffer) - length, "{\"type\": %d", static_cast<unsigned>(i->type));
        length += snprintf(buffer+length, sizeof(buffer) - length, ", \"categroy\": %d", static_cast<unsigned>(i->categroy));
        length += snprintf(buffer+length, sizeof(buffer) - length, ", \"input_from\": %d}", static_cast<unsigned>(i->input_from));
    }
    length += snprintf(buffer+length, sizeof(buffer) - length, "]}\n");

    std::string str(buffer);
    serial->send_bytes(str.c_str(), str.size());
}

void at_get_available_models(const std::string& cmd) {
    auto* serial      = Device::get_device()->get_serial();
    auto* models      = DataDelegate::get_delegate()->get_models_handler();
    auto& models_info = models->get_all_model_info();
    char buffer[512] = {0};
    int length = 0;

    length += snprintf(buffer, sizeof(buffer), "%s\"name\": \"%s\", \"code\": %d, \"data\": [",
        REPLY_CMD_HEADER, cmd.c_str(), static_cast<int>(EL_OK));
    for (const auto& i : models_info) {
        length += snprintf(buffer + length, sizeof(buffer) - length, "%s", model_info_2_json(i));
    }
    length += snprintf(buffer + length, sizeof(buffer) -length, "]}\n");

    std::string str(buffer);
    serial->send_bytes(str.c_str(), str.size());
}

void at_set_model(const std::string& cmd, uint8_t model_id, InferenceEngine* engine, uint8_t& current_model_id) {
    auto* serial        = Device::get_device()->get_serial();
    auto* data_delegate = DataDelegate::get_delegate();
#ifdef CONFIG_EL_LIB_FLASHDB
    auto* storage       = data_delegate->get_storage_handler();
#endif
    auto* models        = data_delegate->get_models_handler();
    char buffer[512] = {0};

    el_model_info_t model_info = models->get_model_info(model_id);
    el_err_code_t   ret        = model_info.id ? EL_OK : EL_EINVAL;
    if (ret != EL_OK) [[unlikely]]
        goto ModelReply;

    // TODO: move heap_caps_malloc to port/el_memory or el_system
    // static auto* tensor_arena = heap_caps_malloc(kTensorArenaSize, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    // memset(tensor_arena, 0, kTensorArenaSize);

    // ret = engine->init(tensor_arena, kTensorArenaSize);
    // if (ret != EL_OK) [[unlikely]]
    //     goto ModelError;

    ret = engine->load_model(model_info.addr_memory, model_info.size);
    if (ret != EL_OK) [[unlikely]]
        goto ModelError;

    if (current_model_id != model_id) {
        current_model_id = model_id;
#ifdef CONFIG_EL_LIB_FLASHDB
        *storage << el_make_storage_kv("current_model_id", current_model_id);
#endif
    }

    goto ModelReply;

ModelError:
    current_model_id = 0;

ModelReply:
    snprintf(buffer, sizeof(buffer), "%s\"name\": \"%d\", \"code\": %d, \"data\": {\"model\": %s}}\n", 
        REPLY_CMD_HEADER, cmd.c_str(), static_cast<int>(ret), model_info_2_json(model_info));

    std::string str(buffer);
    serial->send_bytes(str.c_str(), str.size());
}

void at_get_available_sensors(const std::string& cmd) {
    auto* serial             = Device::get_device()->get_serial();
    auto& registered_sensors = Device::get_device()->get_all_sensor_info();
    char buffer[512] = {0};
    int length = 0;

    length += snprintf(buffer, sizeof(buffer), "%s\"name\": \"%s\", \"code\": %d, \"data\": [",
        REPLY_CMD_HEADER, cmd.c_str(), static_cast<int>(EL_OK));
    for (const auto& i : registered_sensors) {
        length += snprintf(buffer + length, sizeof(buffer) - length, "%s", sensor_info_2_json(i));
    }
    length += snprintf(buffer + length, sizeof(buffer) - length, "]}\n");

    std::string str(buffer);
    serial->send_bytes(str.c_str(), str.size());
}

void at_set_sensor(const std::string& cmd, uint8_t sensor_id, bool enable, uint8_t& current_sensor_id) {
    auto* device      = Device::get_device();
    auto* serial      = device->get_serial();
    auto  sensor_info = device->get_sensor_info(sensor_id);
#ifdef CONFIG_EL_LIB_FLASHDB
    auto* storage     = DataDelegate::get_delegate()->get_storage_handler();
#endif
    char buffer[512] = {0};

    el_err_code_t ret = sensor_info.id ? EL_OK : EL_EINVAL;
    if (ret != EL_OK) [[unlikely]]
        goto SensorReply;

    // camera
    if (sensor_info.type == el_sensor_type_t::EL_SENSOR_TYPE_CAM) {
        auto* camera = device->get_camera();

        device->set_sensor_state(sensor_id, EL_SENSOR_STA_LOCKED);
        if (static_cast<bool>(*camera)) {
            ret = camera->deinit();
            if (ret != EL_OK) [[unlikely]]
                goto SensorError;
        }

        if (enable) [[likely]] {
            ret = camera->init(240, 240);
            if (ret != EL_OK) [[unlikely]]
                goto SensorError;
        }
        device->set_sensor_state(sensor_id, EL_SENSOR_STA_AVAIL);
        sensor_info = device->get_sensor_info(sensor_id);

        if (current_sensor_id != sensor_id) {
            current_sensor_id = sensor_id;
#ifdef CONFIG_EL_LIB_FLASHDB
            *storage << el_make_storage_kv("current_sensor_id", current_sensor_id);
#endif
        }
    } else
        ret = EL_ENOTSUP;
    goto SensorReply;

SensorError:
    current_sensor_id = 0;

SensorReply:
    snprintf(buffer, sizeof(buffer), "%s\"name\": \"%d\", \"code\": %d, \"data\": {\"sensor\": %s}}\n",
        REPLY_CMD_HEADER, cmd.c_str(), static_cast<int>(ret), sensor_info_2_json(sensor_info));

    std::string str(buffer);
    serial->send_bytes(str.c_str(), str.size());
}

void at_run_sample(const std::string& cmd, int n_times, std::atomic<bool>& stop_token, uint8_t current_sensor_id) {
    auto* device      = Device::get_device();
    auto* serial      = device->get_serial();
    auto  sensor_info = device->get_sensor_info(current_sensor_id);
    char buffer[512] = {0};

    el_err_code_t ret = EL_OK;

    auto direct_reply = [&]() {
        memset(buffer, 0x00, sizeof(buffer));
        snprintf(buffer, sizeof(buffer, "%s\"name\": \"%s\", \"code\": %d, \"data\": {\"sensor\": %s}}\n"), 
            REPLY_CMD_HEADER, cmd.c_str(), static_cast<int>(ret), sensor_info_2_json(sensor_info));

        std::string str(buffer);
        serial->send_bytes(str.c_str(), str.size());
    };
    auto event_reply = [&](const std::string& sample_data_str) {
        memset(buffer, 0x00, sizeof(buffer));
        snprintf(buffer, sizeof(buffer, "%s\"name\": \"%s\", \"code\": %d, \"data\": {%s}}\n"), 
            REPLY_EVT_HEADER, cmd.c_str(), static_cast<int>(ret), sample_data_str.c_str());

        std::string str(buffer);
        serial->send_bytes(str.c_str(), str.size());
    };

    ret = sensor_info.id ? EL_OK : EL_EINVAL;
    if (ret != EL_OK) [[unlikely]]
        goto SampleErrorReply;

    if (sensor_info.state != EL_SENSOR_STA_AVAIL) [[unlikely]]
        goto SampleErrorReply;

    if (sensor_info.type == EL_SENSOR_TYPE_CAM) {
        direct_reply();

        auto*    camera = device->get_camera();
        el_img_t img    = el_img_t{.data   = nullptr,
                                   .size   = 0,
                                   .width  = 0,
                                   .height = 0,
                                   .format = EL_PIXEL_FORMAT_UNKNOWN,
                                   .rotate = EL_PIXEL_ROTATE_UNKNOWN};

        while ((n_times < 0 || --n_times >= 0) && !stop_token.load()) {
            ret = camera->start_stream();
            if (ret != EL_OK) [[unlikely]]
                goto SampleLoopErrorReply;

            ret = camera->get_frame(&img);
            if (ret != EL_OK) [[unlikely]]
                goto SampleLoopErrorReply;

            event_reply(img_2_json_str(&img));

            camera->stop_stream();  // Note: discarding return err_code (always EL_OK)
            continue;

        SampleLoopErrorReply:
            event_reply("");
            break;
        }
        return;
    } else
        ret = EL_EINVAL;

SampleErrorReply:
    direct_reply();
}

template <typename AlgorithmType>
void run_invoke_on_img(
  AlgorithmType* algorithm, const std::string& cmd, int n_times, bool result_only, std::atomic<bool>& stop_token) {
    auto*         device          = Device::get_device();
    auto*         camera          = device->get_camera();
    auto*         display         = device->get_display();
    auto*         action_delegate = ActionDelegate::get_delegate();
    auto*         serial          = device->get_serial();
    auto          img             = el_img_t{.data   = nullptr,
                                             .size   = 0,
                                             .width  = 0,
                                             .height = 0,
                                             .format = EL_PIXEL_FORMAT_UNKNOWN,
                                             .rotate = EL_PIXEL_ROTATE_UNKNOWN};
    el_err_code_t ret             = algorithm ? EL_OK : EL_EINVAL;
    auto          event_reply     = [&]() {
        auto str = img_invoke_results_2_json_str(algorithm, &img, cmd, result_only, ret);
        serial->send_bytes(str.c_str(), str.size());
    };
    if (ret != EL_OK) [[unlikely]] {
        event_reply();
        return;
    }
    {
        auto mutable_map = action_delegate->get_mutable_map();
        for (auto& kv : mutable_map) {
            auto argv = tokenize_function_2_argv(kv.first);

            if (!argv.size()) [[unlikely]]
                continue;

            if (argv[0] == "count") {
                // count items by default
                if (argv.size() == 1)
                    kv.second = [=]() -> int {
                        const auto& res = algorithm->get_results();
                        return std::distance(res.begin(), res.end());
                    };

                // count items filtered by id
                if (argv.size() == 3 && argv[1] == "id") {
                    uint8_t target = std::atoi(argv[2].c_str());
                    kv.second      = [=]() -> int {
                        size_t      init = 0;
                        const auto& res  = algorithm->get_results();
                        for (const auto& v : res)
                            if (v.target == target) ++init;
                        return init;
                    };
                }
            }
        }
        action_delegate->set_mutable_map(mutable_map);
    }

    while ((n_times < 0 || --n_times >= 0) && !stop_token.load()) {
        ret = camera->start_stream();
        if (ret != EL_OK) [[unlikely]]
            goto InvokeErrorReply;

        ret = camera->get_frame(&img);
        if (ret != EL_OK) [[unlikely]]
            goto InvokeErrorReply;

        ret = algorithm->run(&img);
        if (ret != EL_OK) [[unlikely]]
            goto InvokeErrorReply;

        draw_results_on_image(algorithm->get_results(), &img);
        ret = display->show(&img);
        if (ret != EL_OK) [[unlikely]]
            goto InvokeErrorReply;

        action_delegate->evalute();

        event_reply();
        camera->stop_stream();  // Note: discarding return err_code (always EL_OK)
        continue;

    InvokeErrorReply:
        camera->stop_stream();
        event_reply();
        break;
    }
}

void at_run_invoke(const std::string& cmd,
                   int                n_times,
                   bool               result_only,
                   std::atomic<bool>& stop_token,
                   InferenceEngine*   engine,
                   uint8_t            current_model_id,
                   uint8_t            current_sensor_id) {
    auto* device             = Device::get_device();
    auto* serial             = device->get_serial();
    auto* algorithm_delegate = AlgorithmDelegate::get_delegate();
    auto* data_delegate      = DataDelegate::get_delegate();
    auto* models             = data_delegate->get_models_handler();
    char buffer[512] = {0};
    int length = 0;

    el_model_info_t     model_info{};
    el_algorithm_info_t algorithm_info{};
    el_sensor_info_t    sensor_info{};

    el_err_code_t ret = EL_OK;

    auto direct_reply = [&](const std::string& algorithm_config) {   
        length += snprintf(buffer, sizeof(buffer), "%s\"name\": \"%s\", \"code\": ", REPLY_CMD_HEADER, 
            cmd.c_str(), static_cast<int>(ret));
        length += snprintf(buffer + length, sizeof(buffer) - length, ", \"data\": {\"model\": %s", 
            model_info_2_json(model_info));
        length += snprintf(buffer + length, sizeof(buffer) - length, ", \"algorithm\": {\"type\": %d", 
            static_cast<unsigned>(algorithm_info.type));
        length += snprintf(buffer + length, sizeof(buffer) - length, ", \"category\": %d, \"config\": {", 
            static_cast<unsigned>(algorithm_info.categroy));
        length += snprintf(buffer + length, sizeof(buffer) - length, "%s}}, \"sensor\": %s}}\n", 
            algorithm_config.c_str(), sensor_info_2_json(sensor_info));
        
        std::string str(buffer);
        serial->send_bytes(str.c_str(), str.size());
    };

    model_info = models->get_model_info(current_model_id);
    if (model_info.id == 0) [[unlikely]]
        goto InvokeErrorReply;

    if (model_info.type != 0)
        algorithm_info = algorithm_delegate->get_algorithm_info(model_info.type);
    else {
        algorithm_info  = algorithm_delegate->get_algorithm_info(el_algorithm_type_from_engine(engine));
        model_info.type = algorithm_info.type;
    }
    if (algorithm_info.type == 0) [[unlikely]]
        goto InvokeErrorReply;

    sensor_info = device->get_sensor_info(current_sensor_id, algorithm_info.input_from);
    if (sensor_info.id == 0 || sensor_info.state != EL_SENSOR_STA_AVAIL) [[unlikely]]
        goto InvokeErrorReply;

    switch (algorithm_info.type) {
    case EL_ALGO_TYPE_IMCLS: {
        std::unique_ptr<AlgorithmIMCLS> algorithm(new AlgorithmIMCLS(engine));

        auto algo_config_helper{AlgorithmConfigHelper<AlgorithmIMCLS>(algorithm.get())};
        direct_reply(algorithm_config_2_json_str(algo_config_helper.dump_config()));

        run_invoke_on_img(algorithm.get(), cmd, n_times, result_only, stop_token);
    }
        return;

    case EL_ALGO_TYPE_FOMO: {
        std::unique_ptr<AlgorithmFOMO> algorithm(new AlgorithmFOMO(engine));

        auto algo_config_helper{AlgorithmConfigHelper<AlgorithmFOMO>(algorithm.get())};
        direct_reply(algorithm_config_2_json_str(algo_config_helper.dump_config()));

        run_invoke_on_img(algorithm.get(), cmd, n_times, result_only, stop_token);
    }
        return;

    case EL_ALGO_TYPE_PFLD: {
        std::unique_ptr<AlgorithmPFLD> algorithm(new AlgorithmPFLD(engine));
        direct_reply(algorithm_config_2_json_str(algorithm->get_algorithm_config()));

        run_invoke_on_img(algorithm.get(), cmd, n_times, result_only, stop_token);
    }
        return;

    case EL_ALGO_TYPE_YOLO: {
        std::unique_ptr<AlgorithmYOLO> algorithm(new AlgorithmYOLO(engine));

        auto algo_config_helper{AlgorithmConfigHelper<AlgorithmYOLO>(algorithm.get())};
        direct_reply(algorithm_config_2_json_str(algo_config_helper.dump_config()));

        run_invoke_on_img(algorithm.get(), cmd, n_times, result_only, stop_token);
    }
        return;

    default:
        ret = EL_ENOTSUP;
    }

InvokeErrorReply:
    ret = EL_EINVAL;
    direct_reply("");
}

void at_set_action(const std::vector<std::string>& argv) {
    auto* serial          = Device::get_device()->get_serial();
#ifdef CONFIG_EL_LIB_FLASHDB
    auto* storage         = DataDelegate::get_delegate()->get_storage_handler();
#endif
    auto* instance        = ReplDelegate::get_delegate()->get_server_handler();
    auto* action_delegate = ActionDelegate::get_delegate();
    char buffer[512] = {0};
    auto  cmd             = std::string{};
    char  action_cmd[128]{};

    el_err_code_t ret = action_delegate->set_condition(argv[1]) ? EL_OK : EL_EINVAL;
    if (ret != EL_OK) [[unlikely]]
        goto ActionReply;

    cmd = argv[2];
    cmd.insert(0, "AT+");
    action_delegate->set_true_cb([=]() {
        el_err_code_t ret = instance->exec_non_lock(cmd);

        char os_buffer[512] = {0};
        snprintf(os_buffer, sizeof(os_buffer), "%s\"name\": \"%s\", \"code\": %d, \"data\": {\"true\"%s}}\n", 
            REPLY_EVT_HEADER, argv[0].c_str(), static_cast<int>(ret), string_2_str(cmd).c_str());

        serial->send_bytes(os_buffer, strlen(os_buffer));
    });
    cmd = argv[3];
    cmd.insert(0, "AT+");
    action_delegate->set_false_exception_cb([=]() { instance->exec_non_lock(cmd); });

    {
        char builder[512] = {0};
        snprintf(builder, sizeof(builder), "AT+%s=%s,%s,%s", 
            string_2_str(argv[1]).c_str(), string_2_str(argv[2]).c_str(), string_2_str(argv[3]).c_str());
        
        std::string str(builder);
        cmd = str;
        std::strncpy(action_cmd, cmd.c_str(), sizeof(action_cmd) - 1);
#ifdef CONFIG_EL_LIB_FLASHDB
        auto kv = el_make_storage_kv("action_cmd", action_cmd);
        *storage << kv;
#endif
    }

ActionReply:
    snprintf(buffer, sizeof(buffer), "%s\"name\": \"%s\", \"code\": %d, \"data\": {\"cond\": %s, \
        \"true\": %s, \"false_or_exception\": %s}}\n", 
        REPLY_CMD_HEADER, argv[0].c_str(), static_cast<int>(ret), string_2_str(argv[1]).c_str(), 
        string_2_str(argv[2]).c_str(), string_2_str(argv[3]).c_str());

    serial->send_bytes(buffer, strlen(buffer));
}

void at_unset_action(const std::string& cmd) {
    auto* serial          = Device::get_device()->get_serial();
#ifdef CONFIG_EL_LIB_FLASHDB
    auto* storage         = DataDelegate::get_delegate()->get_storage_handler();
#endif
    auto* action_delegate = ActionDelegate::get_delegate();
    char buffer[512] = {0};

    action_delegate->unset_condition();
#ifdef CONFIG_EL_LIB_FLASHDB
    storage->erase("action_cmd");
#endif

    snprintf(buffer, sizeof(buffer), "%s\"name\": \"%s\", \"code\": %d, \"data\": {}}\n", 
        REPLY_CMD_HEADER, cmd.c_str(), static_cast<int>(EL_OK));

    std::string str(buffer);
    serial->send_bytes(str.c_str(), str.size());
}

void at_get_action(const std::string& cmd) {
    auto* serial          = Device::get_device()->get_serial();
#ifdef CONFIG_EL_LIB_FLASHDB
    auto* storage         = DataDelegate::get_delegate()->get_storage_handler();
#endif
    auto* action_delegate = ActionDelegate::get_delegate();
    char buffer[512] = {0};
    char  action_cmd[128]{};

    if (action_delegate->has_condition() 
#ifdef CONFIG_EL_LIB_FLASHDB
    && storage->contains("action_cmd")
#endif
    ) {
#ifdef CONFIG_EL_LIB_FLASHDB
        *storage >> el_make_storage_kv("action_cmd", action_cmd);
#endif
    }

    std::string action_cmd_str = action_cmd;
    snprintf(buffer, sizeof(buffer), "%s\"name\": \"%s\", \"code\": %d, \"data\": %s}\n", 
        REPLY_CMD_HEADER, cmd.c_str(), static_cast<int>(EL_OK), string_2_str(action_cmd).c_str());

    std::string str(buffer);
    serial->send_bytes(str.c_str(), str.size());
}


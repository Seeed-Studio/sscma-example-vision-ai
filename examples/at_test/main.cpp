#include "edgelab.h"

#define REPLY_CMD_HEADER "\r{\"type\": 0, "
#define REPLY_LOG_HEADER "\r{\"type\": 2, "

void at_print_help(std::forward_list<el_repl_cmd_t> cmd_list) {
    auto* serial = Device::get_device()->get_serial();
    char buffer[512] = {0};
    int length = 0;

    for (const auto& cmd : cmd_list) {
        length += snprintf(buffer + length, sizeof(buffer), "  AT+%s", cmd.cmd.c_str());
        if (cmd.args.size()) {
            length += snprintf(buffer + length, sizeof(buffer), "=<%s>", cmd.args.c_str());
        }
        length += snprintf(buffer + length, sizeof(buffer), "\n    %s\n", cmd.desc.c_str());
    }

    std::string str(buffer);
    serial->send_bytes(str.c_str(), str.size());
}

void at_server_echo_cb(el_err_code_t ret, const std::string& msg) {
    auto* serial = Device::get_device()->get_serial();
    char buffer[512] = {0};

    if (ret != EL_OK) {
        snprintf(buffer, sizeof(buffer), "%s\"name\": \"AT\", \"code\": %d, \"data\": %s}\n", 
            REPLY_LOG_HEADER, (int)ret, msg.c_str());
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

int main()
{
    uint32_t frame = 0;

    Device* device  = Device::get_device();
    Serial* serial  = device->get_serial();
    auto* repl          = ReplDelegate::get_delegate();
    auto* instance      = repl->get_server_handler();
    auto* executor      = repl->get_executor_handler();

    //TODO: need to fix
    //add this delay for console output ready
    el_sleep(500);

    el_err_code_t ret = EL_OK;
    ret = serial->init();
    el_printf("serial init=%d\r\n", ret);
    instance->init(at_server_echo_cb);

    // register repl commands (overrite help)
    instance->register_cmd("HELP", "List available commands", "", [&](std::vector<std::string> argv) {
        auto registered_cmds = instance->get_registered_cmds();
        at_print_help(registered_cmds);
        return EL_OK;
    });

    instance->register_cmd("ID?", "Get device ID", "", el_repl_cmd_cb_t([&](std::vector<std::string> argv) {
                               at_get_device_id(argv[0]);
                               return EL_OK;
                           }));

    // start task executor
    executor->start();

    char* buf = new char[128]{};
    for (;;)
    {
        el_sleep(20);
        serial->get_line(buf, 128);
        instance->exec(buf);
    }
    delete[] buf;

    return 0;
}
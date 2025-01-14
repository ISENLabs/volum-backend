#include <iostream>
#include <rapidjson/document.h>

namespace Proxmox{
class Requests{
private:
    std::string do_proxmox_request();
public:
    rapidjson::Document list_lxcs();
    rapidjson::Document get_lxc(uint pct_id);
    rapidjson::Document get_lxc_interfaces(uint pct_id);
    rapidjson::Document create_lxc(std::string payload);
    rapidjson::Document delete_lxc(uint pct_id);
    rapidjson::Document stop_lxc(uint pct_id);
    rapidjson::Document start_lxc(uint pct_id);

    // TODO: Firewall, but we'll do it later
    rapidjson::Document get_lxc_firewall(uint pct_id);
    rapidjson::Document update_lxc_firewall(uint pct_id, std::string payload);
};
};
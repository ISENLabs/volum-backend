#include <iostream>
#include <rapidjson/document.h>
#include "../utils/http.hpp"

using namespace Utils::Http;

namespace Proxmox{
    class Requests{
    private:
        Client* client;

        std::string do_proxmox_request();
    public:
        Requests();
        ~Requests();
        rapidjson::Document response_to_document(Response resp);

        rapidjson::Document list_lxcs();
        rapidjson::Document get_lxc(uint pct_id);
        rapidjson::Document get_lxc_interfaces(uint pct_id);
        bool create_lxc(std::string payload);
        bool delete_lxc(uint pct_id);
        bool stop_lxc(uint pct_id);
        bool start_lxc(uint pct_id);

        // TODO: Firewall, but we'll do it later
        rapidjson::Document get_lxc_firewall(uint pct_id);
        rapidjson::Document update_lxc_firewall(uint pct_id, std::string payload);
    };
};
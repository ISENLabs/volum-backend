#include "methods.hpp"
#include <rapidjson/stringbuffer.h>
#include <rapidjson/prettywriter.h>
using namespace Proxmox::Structs;
using Proxmox::Methods;

Proxmox_LXC Methods::get_lxc(uint pct_id){
    Proxmox::Requests req;
    rapidjson::Document _lxc = req.get_lxc(pct_id);

    // Extract $.data
    if(_lxc.HasMember("data") && _lxc["data"].IsObject()){
        Proxmox_LXC lxc = Converters::json_to_lxc(_lxc["data"]);
        std::string ipv4 = get_pv_ipv4(pct_id);
        lxc.ip_address = ipv4;
        return lxc;
    }
    throw std::runtime_error("Can't get $.data");
}

std::string Methods::get_pv_ipv4(uint pct_id){
    Proxmox::Requests req;
    rapidjson::Document _interfaces = req.get_lxc_interfaces(pct_id);

    // Extract $.data
    if(_interfaces.HasMember("data") && _interfaces["data"].IsArray()){
        rapidjson::Value& val = _interfaces["data"];

        // Find the right interface
        auto arr = val.GetArray();
        auto it = arr.Begin();
        while(it != arr.End()){
            if(it->HasMember("name") && it->HasMember("inet") && (*it)["name"].IsString() && std::string((*it)["name"].GetString()) == "eth0"){
                // Strip the subnet
                std::string _ipv4 = (*it)["inet"].GetString();
                size_t _pos = _ipv4.find_first_of('/');
                return _ipv4.substr(0, _pos);
            }
            it++;
        }
    }

    throw std::runtime_error("Can't extract ipv4");
}

Proxmox_LXCS Methods::get_lxcs(){
    Proxmox_LXCS lxcs;
    return lxcs;
}
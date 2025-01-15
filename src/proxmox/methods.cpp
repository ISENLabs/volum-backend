#include "methods.hpp"

using namespace Proxmox::Structs;
using Proxmox::Methods;

Proxmox_LXC Methods::get_lxc(uint pct_id){
    Proxmox::Requests req;
    rapidjson::Document& _lxc = req.get_lxc(pct_id);
    
    // Extract $.data
    if(_lxc.HasMember("data") && _lxc["data"].IsObject()){
        Proxmox_LXC lxc = Converters::json_to_lxc((rapidjson::Value&)_lxc["data"]);
        return lxc;
    }
    throw std::runtime_error("Can't get $.data");
}

Proxmox_LXCS Methods::get_lxcs(){
    Proxmox_LXCS lxcs;
    return lxcs;
}
#include "methods.hpp"
#include <rapidjson/stringbuffer.h>
#include <rapidjson/prettywriter.h>
#include "../services/database.hpp"
#include "../utils/config.hpp"

#include <iostream>
#include <set>

using namespace Proxmox::Structs;
using namespace Utils::Config;
using Proxmox::Methods;
using Services::Database;

std::string escape_string(const std::string& str) {
    std::string result;
    for (char c : str) {
        switch (c) {
            case ' ':
                result += "%20";
                break;
            case '&':
                result += "%26";
                break;
            case '=':
                result += "%3D";
                break;
            case '%':
                result += "%25";
                break;
            case ':':
                result += "%3A";
                break;
            case '/':
                result += "%2F";
                break;
            case ',':
                result += "%2C";
                break;
            default:
                result += c;
        }
    }
    return result;
}

Proxmox_LXC Methods::get_lxc(uint pct_id) {
    auto& cache = get_cache();
    auto _elt = cache.get_element(pct_id, false);
    if(_elt.has_value()){
        return *_elt;
    }

    Proxmox::Requests req;
    rapidjson::Document _lxc = req.get_lxc(pct_id);

    // Extract $.data
    if(_lxc.HasMember("data") && _lxc["data"].IsObject()){
        Proxmox_LXC lxc = Converters::json_to_lxc(_lxc["data"]);

        // Store in cache
        cache.store_element(lxc.vm_id, lxc);
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
    Proxmox::Requests req;
    rapidjson::Document _lxcs = req.list_lxcs();

    Proxmox_LXCS lxcs = Converters::json_to_lxcs(_lxcs);

    // Store lxcs in cache
    auto& cache = get_cache();
    for(auto lxc : lxcs){
        cache.store_element(lxc.vm_id, lxc);
    }

    return lxcs;
}

Proxmox_LXC Methods::create_lxc(uint pct_id, std::string ct_name, std::string ip, std::string password) {
    auto& config = Env_Struct::getInstance();
    std::string payload = "vmid=" + std::to_string(pct_id) + 
        "&hostname=" + escape_string(ct_name) + 
        "&ostemplate=" + escape_string(config.vm_osTemplate) + 
        "&onboot=" + (config.vm_onboot ? "1" : "0") + 
        "&cores=" + std::to_string(config.vm_cores) + 
        "&memory=" + std::to_string(config.vm_memory) + 
        "&swap=512" + // by default
        "&rootfs=" + escape_string(config.vm_storage + ":" + std::to_string(config.vm_disk)) + 
        "&net0=" + escape_string(std::string("name=eth0,bridge=" + config.vm_bridge + ",rate=" + std::to_string(config.vm_netSpeed))) + 
              escape_string(",ip=" + ip + "/32") + 
              escape_string(",gw=" + config.vm_gateway) + 
        "&unprivileged=1" +
        "&features=nesting%3D1" +
        "&bwlimit=" + std::to_string(config.vm_ioSpeed) + 
        "&password=" + escape_string(password) + 
        "&start=1&ssh-public-keys=";

    Proxmox::Requests req;
    req.create_lxc(payload);

    Proxmox_LXC lxc = get_lxc(pct_id);
    if(config.vm_onboot) 
        lxc.status = Proxmox_LXC_State::RUNNING;

    // Add to cache
    auto& cache = get_cache();
    cache.store_element(lxc.vm_id, lxc);

    return lxc;
}

bool Methods::delete_lxc(uint pct_id) {
    Proxmox::Requests req;
    if(!req.delete_lxc(pct_id)) 
        return false;

    // Remove from cache
    auto& cache = get_cache();
    cache.remove(pct_id);

    return true;
}

bool Methods::stop_lxc(uint pct_id) {
    Proxmox::Requests req;
    if(!req.stop_lxc(pct_id)) return false;

    auto& cache = get_cache();
    Proxmox_LXC lxc = get_lxc(pct_id);
    lxc.status = Proxmox_LXC_State::STOPPED;
    cache.store_element(lxc.vm_id, lxc);

    return true;
}

bool Methods::start_lxc(uint pct_id) {
    Proxmox::Requests req;
    if(!req.start_lxc(pct_id)) return false;

    auto& cache = get_cache();
    Proxmox_LXC lxc = get_lxc(pct_id);
    lxc.status = Proxmox_LXC_State::RUNNING;
    cache.store_element(lxc.vm_id, lxc);

    return true;
}

CacheHandler<uint, Proxmox_LXC>& Methods::get_cache(){
    static CacheHandler<uint, Proxmox_LXC> cache_inst(PROXMOX_LXC_CACHE_TTL);
    return cache_inst;
}
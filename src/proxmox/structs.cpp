#include "../utils/debug.hpp"
#include "structs.hpp"
#include <exception>

using namespace Proxmox::Structs;
using Utils::Debug;

std::string Converters::lxc_to_json(Proxmox_LXC lxc){
    std::string json = "{"
        "\"status\": \"" + std::string((lxc.status == Proxmox_LXC_State::RUNNING ? "running" : "stopped")) + "\","
        "\"name\": \"" + lxc.name + "\","
        "\"vmid\": " + std::to_string(lxc.vm_id) + ","
        "\"cpus\": " + std::to_string(lxc.cpus) + ","
        "\"maxmem\": " + std::to_string(lxc.maxmem) + ","
        "\"maxdisk\": " + std::to_string(lxc.maxdisk) + ","
        "\"maxswap\": " + std::to_string(lxc.maxswap) + ","
        "\"uptime\": " + std::to_string(lxc.uptime) + ","
        "\"cpu\": " + std::to_string(lxc.cpu) + ","
        "\"disk\": " + std::to_string(lxc.disk) + ","
        "\"mem\": " + std::to_string(lxc.mem) + ","
        "\"swap\": " + std::to_string(lxc.swap) + ","
        "\"netin\": " + std::to_string(lxc.net_in) + ","
        "\"netout\": " + std::to_string(lxc.net_out);

    if(lxc.ip_address != ""){
        json += ",\"ip_address\":\""+ lxc.ip_address +"\"";
    }

    if(lxc.subdomain != ""){
        json += ",\"subdomain\":\""+ lxc.subdomain +"\"";
    }

    if(lxc.password != ""){
        json += ",\"password\":\""+ lxc.password +"\"";
    }

    if(lxc.owner_id > 0){
        json += ",\"owner_id\":\""+ std::to_string(lxc.owner_id) +"\"";
    }

    if(lxc.owner_name != ""){
        json += ",\"owner_name\":\""+ lxc.owner_name +"\"";
    }

    if(lxc.owner_email != ""){
        json += ",\"owner_email\":\""+ lxc.owner_email +"\"";
    }

    if(lxc.owner_class != ""){
        json += ",\"owner_class\":\""+ lxc.owner_class +"\"";
    }

    json += "}";
    return json;
}

std::string Converters::lxcs_to_json(Proxmox_LXCS lxc){
    std::string json = "[";
    for(auto& _lxc : lxc){
        json += lxc_to_json(_lxc) + ",";
    }
    if(lxc.size()) // Only remove the trailing comma if one or more elt is added in the json
        json.pop_back();
    json += "]";
    return json;
}

/// @brief Parse json and extract the struct. Only call it with the $.data.
/// @param json 
/// @return 
Proxmox_LXC Converters::json_to_lxc(rapidjson::Value& json){
    try{
        Proxmox_LXC lxc;
        // Check if the keys exist
        if(json.HasMember("status") && json["status"].IsString())
            lxc.status = std::string(json["status"].GetString()) == "running" ? Proxmox_LXC_State::RUNNING : Proxmox_LXC_State::STOPPED;
        else
            throw std::runtime_error("Unable to get status value");

        if(json.HasMember("name") && json["name"].IsString())
            lxc.name = json["name"].GetString();
        else
            throw std::runtime_error("Unable to get name value");

        if(json.HasMember("vmid") && json["vmid"].IsUint())
            lxc.vm_id = json["vmid"].GetUint();
        else
            throw std::runtime_error("Unable to get vmid value");

        if(json.HasMember("cpus") && json["cpus"].IsUint())
            lxc.cpus = json["cpus"].GetUint();
        else
            throw std::runtime_error("Unable to get cpus value");

        if(json.HasMember("maxmem") && json["maxmem"].IsUint64())
            lxc.maxmem = json["maxmem"].GetUint64();
        else
            throw std::runtime_error("Unable to get status value");

        if(json.HasMember("maxdisk") && json["maxdisk"].IsUint64())
            lxc.maxdisk = json["maxdisk"].GetUint64();
        else
            throw std::runtime_error("Unable to get maxdisk value");

        if(json.HasMember("maxswap") && json["maxswap"].IsUint64())
            lxc.maxswap = json["maxswap"].GetUint64();
        else
            throw std::runtime_error("Unable to get maxswap value");

        if(json.HasMember("uptime") && json["uptime"].IsUint())
            lxc.uptime = json["uptime"].GetUint();
        else
            throw std::runtime_error("Unable to get uptime value");

        if(json.HasMember("cpu") && json["cpu"].IsDouble())
            lxc.cpu = json["cpu"].GetDouble();
        else
            lxc.cpu = 0;

        if(json.HasMember("disk") && json["disk"].IsUint64())
            lxc.disk = json["disk"].GetUint64();
        else
            throw std::runtime_error("Unable to get disk value");

        if(json.HasMember("mem") && json["mem"].IsUint64())
            lxc.mem = json["mem"].GetUint64();
        else
            throw std::runtime_error("Unable to get mem value");

        if(json.HasMember("swap") && json["swap"].IsUint64())
            lxc.swap = json["swap"].GetUint64();
        else
            throw std::runtime_error("Unable to get swap value");

        if(json.HasMember("netin") && json["netin"].IsUint64())
            lxc.net_in = json["netin"].GetUint64();
        else
            throw std::runtime_error("Unable to get net_in value");

        if(json.HasMember("netout") && json["netout"].IsUint64())
            lxc.net_out = json["netout"].GetUint64();
        else
            throw std::runtime_error("Unable to get net_out value");

        return lxc;
    }
    catch(std::exception& ex){
        Debug::Log("Unable to deserialize json: " + std::string(ex.what()), "CV-JTL");
        throw ex;
    }
}

Proxmox_LXCS Converters::json_to_lxcs(rapidjson::Value& json){
    if(!json.HasMember("data") || !json["data"].IsArray()){
        Debug::Log("Unable to extract LXC from json.", "CV-JTLS");
        throw std::runtime_error("Unable to parse json");
    }

    rapidjson::Value& _data = json["data"];
    rapidjson::GenericArray<false, rapidjson::Value> _array = _data.GetArray();

    // Create the vector
    Proxmox_LXCS vector;
    vector.reserve(_array.Size());
    // Populate it 
    for(auto& value : _array){
        Proxmox_LXC _lxc = json_to_lxc(value);
        vector.push_back(_lxc);
    }

    return vector;
}
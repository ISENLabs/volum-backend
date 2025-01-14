#include "requests.hpp"
#include <iostream>

#include "../utils/config.hpp"
#include "../utils/debug.hpp"

using namespace Proxmox;
using namespace Utils::Config;
using Utils::Debug;

Requests::Requests() {
    auto& config = Env_Struct::getInstance();
    client = new Client(config.pve_host.c_str());

    std::string token = "PVEAPIToken="+config.pve_tokenId+"="+config.pve_tokenSecret;
    client->set_header("Authorization", token.c_str());

    //Debug::addIgnore("PVE-REQUEST-ERROR");
    //Debug::addIgnore("PVE-REQUEST");
}

Requests::~Requests() {
    delete client;
}

rapidjson::Document Requests::response_to_document(Response resp) {
    rapidjson::Document doc;

    if(resp.error) {
        Debug::Log(resp.error_reason.c_str(), "PVE-REQUEST-ERROR");
        return doc;
    }

    doc.Parse(resp.body.c_str());
    if(doc.HasParseError()) {
        Debug::Log("JSON Parse Error: " + std::to_string(doc.GetParseError()), "PVE-REQUEST-ERROR");
        return doc;
    }

    Debug::Log(resp.body.c_str(), "PVE-REQUEST");
    if(doc.HasMember("data")) {
        rapidjson::Document finalDoc;
        finalDoc.CopyFrom(doc["data"], finalDoc.GetAllocator());

        return finalDoc;
    }

    return doc;
}

rapidjson::Document Requests::list_lxcs() {
    auto& config = Env_Struct::getInstance();
    std::string path = "/api2/json/nodes/"+config.pve_node+"/lxc/";
    Response resp = client->get(path.c_str());

    return response_to_document(resp);
}


rapidjson::Document Requests::get_lxc(uint pct_id) {
    auto& config = Env_Struct::getInstance();

    std::string path = "/api2/json/nodes/"+config.pve_node+"/lxc/"+std::to_string(pct_id)+"/status/current";
    Response resp = client->get(path.c_str());

    return response_to_document(resp);
}

rapidjson::Document Requests::get_lxc_interfaces(uint pct_id) {
    auto& config = Env_Struct::getInstance();

    std::string path = "/api2/json/nodes/"+config.pve_node+"/lxc/"+std::to_string(pct_id)+"/interfaces";
    Response resp = client->get(path.c_str());

    return response_to_document(resp);
}

rapidjson::Document Requests::create_lxc(std::string payload) {
    auto& config = Env_Struct::getInstance();

    std::string path = "/api2/json/nodes/"+config.pve_node+"/lxc";
    Response resp = client->post(path.c_str(), payload.c_str());

    return response_to_document(resp);
}

rapidjson::Document Requests::delete_lxc(uint pct_id) {
    auto& config = Env_Struct::getInstance();

    std::string path = "/api2/json/nodes/"+config.pve_node+"/lxc/"+std::to_string(pct_id);
    std::string body = "node="+config.pve_node+"&vmid="+std::to_string(pct_id);
    Response resp = client->del(path.c_str(), body.c_str());

    return response_to_document(resp);
}

rapidjson::Document Requests::stop_lxc(uint pct_id) {
    auto& config = Env_Struct::getInstance();

    std::string path = "/api2/json/nodes/"+config.pve_node+"/lxc/"+std::to_string(pct_id)+"/status/stop";
    std::string body = "node="+config.pve_node+"&vmid="+std::to_string(pct_id);
    Response resp = client->post(path.c_str(), body.c_str());

    return response_to_document(resp);
}

rapidjson::Document Requests::start_lxc(uint pct_id) {
    auto& config = Env_Struct::getInstance();

    std::string path = "/api2/json/nodes/"+config.pve_node+"/lxc/"+std::to_string(pct_id)+"/status/start";
    std::string body = "node="+config.pve_node+"&vmid="+std::to_string(pct_id);
    Response resp = client->post(path.c_str(), body.c_str());

    return response_to_document(resp);
}

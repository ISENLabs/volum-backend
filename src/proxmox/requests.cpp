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
    client->set_timeout(10);

    std::string token = "PVEAPIToken="+config.pve_tokenId+"="+config.pve_tokenSecret;
    client->set_header("Authorization", token.c_str());

    client->set_cert_validation(false);

    // Debug::addIgnore("PVE-RQ-ERR");
    Debug::addIgnore("PVE-RQ");
}

Requests::~Requests() {
    delete client;
}

rapidjson::Document Requests::response_to_document(Response resp) {
    rapidjson::Document doc;

    if(resp.error) {
        Debug::Log("Error in resp: " + resp.error_reason, "PVE-RQ-ERR");
        throw std::runtime_error("Error while extracting data");
    }

    doc.Parse(resp.body.c_str());
    if(doc.HasParseError()) {
        Debug::Log("JSON Parse Error: " + std::to_string(doc.GetParseError()), "PVE-RQ-ERR");
        throw std::runtime_error("Error while extracting data");
    }

    Debug::Log(resp.body.c_str(), "PVE-RQ");

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

bool Requests::create_lxc(std::string payload) {
    auto& config = Env_Struct::getInstance();

    std::string path = "/api2/json/nodes/"+config.pve_node+"/lxc";
    Response resp = client->post(path.c_str(), payload.c_str());

    if(resp.body.length() == 0) return false;
    return true;
}

bool Requests::delete_lxc(uint pct_id) {
    auto& config = Env_Struct::getInstance();

    std::string path = "/api2/json/nodes/"+config.pve_node+"/lxc/"+std::to_string(pct_id);
    std::string body = "";
    Response resp = client->del(path.c_str(), body.c_str());

    if(resp.body.length() == 0) return false;
    return true;
}

bool Requests::stop_lxc(uint pct_id) {
    auto& config = Env_Struct::getInstance();

    std::string path = "/api2/json/nodes/"+config.pve_node+"/lxc/"+std::to_string(pct_id)+"/status/stop";
    std::string body = "{\"vmid\":"+std::to_string(pct_id)+", \"node\":\""+config.pve_node+"\"}";
    Response resp = client->post(path.c_str(), body.c_str());

    if(resp.body.length() == 0) return false;
    return true;
}

bool Requests::start_lxc(uint pct_id) {
    auto& config = Env_Struct::getInstance();

    std::string path = "/api2/json/nodes/"+config.pve_node+"/lxc/"+std::to_string(pct_id)+"/status/start";
    std::string body = "{\"vmid\":"+std::to_string(pct_id)+", \"node\":\""+config.pve_node+"\"}";
    Response resp = client->post(path.c_str(), body.c_str());

    if(resp.body.length() == 0) return false;
    return true;
}

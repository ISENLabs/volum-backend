#include "config.hpp"
#include "debug.hpp"
#include <exception>
#include <fstream>

using Utils::Config::Env_Struct;
using Utils::Debug;

Env_Struct& Env_Struct::getInstance(){
    static Env_Struct instance;

    if(!instance._initialized){
        // First initialization: parse the config. 
        try{
            instance.init_parseConfig();
            instance._initialized = true;
        }
        catch(std::exception& ex){
            Debug::Log("Unable to parse config: " + std::string(ex.what()), "ES-GETINST");
            exit(1);
        }
    }
    return instance;
}

void Env_Struct::init_checkConfig(rapidjson::Document& doc){
    // For each element, check if it exists + type before assigning it
    if(doc.HasMember("pve")){
        const rapidjson::Value& pve = doc["pve"];
        if(pve.HasMember("host") && pve["host"].IsString())
            pve_host = pve["host"].GetString();
        else
            throw std::runtime_error("Missing pve.host in config");

        if(pve.HasMember("token_id") && pve["token_id"].IsString())
            pve_tokenId = pve["token_id"].GetString();
        else
            throw std::runtime_error("Missing pve.token_id in config");

        if(pve.HasMember("token_secret") && pve["token_secret"].IsString())
            pve_tokenSecret = pve["token_secret"].GetString();
        else
            throw std::runtime_error("Missing pve.token_secret in config");

        if(pve.HasMember("node") && pve["node"].IsString())
            pve_node = pve["node"].GetString();
        else
            throw std::runtime_error("Missing pve.node in config");

    }
    else
        throw std::runtime_error("Missing pve in config");

    if(doc.HasMember("vms_options")){
        const rapidjson::Value& vms = doc["vms_options"];
        if(vms.HasMember("start_index") && vms["start_index"].IsUint())
            vm_startIndex = vms["start_index"].GetUint();
        else
            throw std::runtime_error("Missing vms_options.start_index in config");

        if(vms.HasMember("start_ip") && vms["start_ip"].IsString())
            vm_startIp = vms["start_ip"].GetString();
        else
            throw std::runtime_error("Missing vms_options.start_ip in config");
    
        if(vms.HasMember("os_template") && vms["os_template"].IsString())
            vm_osTemplate = vms["os_template"].GetString();
        else
            throw std::runtime_error("Missing vms_options.os_template in config");

        if(vms.HasMember("onboot") && vms["onboot"].IsBool())
            vm_onboot = vms["onboot"].GetBool();
        else
            throw std::runtime_error("Missing vms_options.onboot in config");

        if(vms.HasMember("cores") && vms["cores"].IsUint())
            vm_cores = vms["cores"].GetUint();
        else
            throw std::runtime_error("Missing vms_options.cores in config");

        if(vms.HasMember("memory") && vms["memory"].IsUint())
            vm_memory = vms["memory"].GetUint();
        else
            throw std::runtime_error("Missing vms_options.memory in config");

        if(vms.HasMember("disk") && vms["disk"].IsUint())
            vm_disk = vms["disk"].GetUint();
        else
            throw std::runtime_error("Missing vms_options.disk in config");

        if(vms.HasMember("storage") && vms["storage"].IsString())
            vm_storage = vms["storage"].GetString();
        else
            throw std::runtime_error("Missing vms_options.storage in config");

        if(vms.HasMember("net_speed") && vms["net_speed"].IsUint())
            vm_netSpeed = vms["net_speed"].GetUint();
        else
            throw std::runtime_error("Missing vms_options.net_speed in config");

        if(vms.HasMember("io_speed") && vms["io_speed"].IsUint())
            vm_ioSpeed = vms["io_speed"].GetUint();
        else
            throw std::runtime_error("Missing vms_options.io_speed in config");

        if(vms.HasMember("gateway") && vms["gateway"].IsString())
            vm_gateway = vms["gateway"].GetString();
        else
            throw std::runtime_error("Missing vms_options.gateway in config");

        if(vms.HasMember("bridge") && vms["bridge"].IsString())
            vm_bridge = vms["bridge"].GetString();
        else
            throw std::runtime_error("Missing vms_options.bridge in config");
    }
    else
        throw std::runtime_error("Missing vms_options in config");

    if(doc.HasMember("db_options")){
        const rapidjson::Value& db = doc["db_options"];
        if(db.HasMember("host") && db["host"].IsString())
            db_host = db["host"].GetString();
        else
            throw std::runtime_error("Missing db_options.host in config");

        if(db.HasMember("port") && db["port"].IsUint())
            db_port = db["port"].GetUint();
        else
            throw std::runtime_error("Missing db_options.port in config");

        if(db.HasMember("user") && db["user"].IsString())
            db_user = db["user"].GetString();
        else
            throw std::runtime_error("Missing db_options.user in config");

        if(db.HasMember("password") && db["password"].IsString())
            db_password = db["password"].GetString();
        else
            throw std::runtime_error("Missing db_options.password in config");

        if(db.HasMember("database") && db["database"].IsString())
            db_name = db["database"].GetString();
        else
            throw std::runtime_error("Missing db_options.database in config");
    }
    else
        throw std::runtime_error("Missing db_options in config");

    if(doc.HasMember("auth")){
        const rapidjson::Value& auth = doc["auth"];
        if(auth.HasMember("lazy_hostname") && auth["lazy_hostname"].IsString())
            auth_lazyHostname = auth["lazy_hostname"].GetString(); // Let's assume the user entered a well-formated hostname
        else
            throw std::runtime_error("Missing auth.lazy_hostname in config");

        if(auth.HasMember("lazy_route") && auth["lazy_route"].IsString())
            auth_lazyRoute = auth["lazy_route"].GetString(); // Let's assume the user entered a well-formated route
        else
            throw std::runtime_error("Missing auth.lazy_route in config");

        if(auth.HasMember("cookie_name") && auth["cookie_name"].IsString())
            auth_cookieName = auth["cookie_name"].GetString(); // Let's assume the user entered a well-formated route
        else
            throw std::runtime_error("Missing auth.cookie_name in config");
    }
    else
        throw std::runtime_error("Missing auth in config");

    if(doc.HasMember("webserver")){
        const rapidjson::Value& ws = doc["webserver"];
        if(ws.HasMember("port") && ws["port"].IsUint())
            webserver_port = ws["port"].GetUint();
        else
            throw std::runtime_error("Missing webserver.port in config");
    }
    else
        throw std::runtime_error("Missing webserver in config");
}

void Env_Struct::init_parseConfig(){
    // Check if file "env.json" exists
    std::ifstream file("env.json");
    if(!file.good()){
        throw std::runtime_error("File env.json not found");
    }

    // Read the file
    std::string content;
    std::string line;
    while(std::getline(file, line)){
        content += line;
    }

    // Parse the JSON
    rapidjson::Document doc;
    doc.Parse(content.c_str());
    
    if(doc.HasParseError()){
        throw std::runtime_error("Failed to parse JSON");
    }

    // Call the checkConfig method
    init_checkConfig(doc);
}

void Env_Struct::set_auth_handler(std::shared_ptr<Services::Auth::IAuthentication> auth_provider){
    _auth_provider = auth_provider;
    _provider_set = true;
}

std::shared_ptr<Services::Auth::IAuthentication> Env_Struct::get_auth_provider(){
    if(!_provider_set)
        throw std::runtime_error("You should set_auth_handler() before getting the auth handler.");

    return _auth_provider;
}
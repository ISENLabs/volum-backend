#pragma once
#include <iostream>
#include <rapidjson/document.h>
#include "../services/auth.hpp"

namespace Utils::Config {

// Singleton, so we can access it everywhere
class Env_Struct {
public:
    /**
     *   Env elements
     */
    std::string pve_host;
    std::string pve_tokenId;
    std::string pve_tokenSecret;
    std::string pve_node;

    uint vm_startIndex;
    std::string vm_startIp;
    std::string vm_osTemplate;
    bool vm_onboot;
    ushort vm_cores;
    ushort vm_memory;
    ushort vm_disk; 
    std::string vm_storage;
    ushort vm_netSpeed;
    ushort vm_ioSpeed;
    std::string vm_gateway;
    std::string vm_bridge;

    std::string db_host;
    ushort db_port;
    std::string db_user;
    std::string db_password;
    std::string db_name;

    std::string auth_lazyHostname;
    std::string auth_lazyRoute;
    std::string auth_cookieName;
    std::string auth_corsFrontend;

    uint webserver_port;

    /** 
     * Method
     */
    bool parseConfig();
    static Env_Struct& getInstance();
    void set_auth_handler(std::shared_ptr<Services::Auth::IAuthentication> auth_provider);
    std::shared_ptr<Services::Auth::IAuthentication> get_auth_provider();

private:
    bool _initialized = false;
    bool _provider_set = false; // Could be avoided by checking directly if _auth_provider is nullptr, but 
    std::shared_ptr<Services::Auth::IAuthentication> _auth_provider;

    void init_parseConfig();
    void init_checkConfig(rapidjson::Document& doc);
    
    // Singleton
    Env_Struct() : _auth_provider(nullptr) {};
    Env_Struct(Env_Struct const*);
    void operator=(Env_Struct const&);

}; // Class Env_Struct

}; // Namespace Config
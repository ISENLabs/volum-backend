#include <iostream>
#include <rapidjson/document.h>

namespace Utils::Config {

// Singleton, so we can access it everywhere
class Env_Struct {
public:
    /**
     *   Env elements
     */
    std::string pve_host;
    std::string pve_token;

    uint vm_startIndex;
    std::string vm_osTemplate;
    std::string vm_node;
    bool vm_onboot;
    ushort vm_cores;
    ushort vm_memory;
    ushort vm_disk; 
    std::string vm_storage;
    ushort vm_netSpeed;
    ushort vm_ioSpeed;

    std::string db_host;
    ushort db_port;
    std::string db_user;
    std::string db_password;
    std::string db_name;

    std::string auth_lazyApi;


    /**
     * Class-Specific elements
     */
    bool initialized = false;
    
    /** 
     * Method
     */
    bool parseConfig();
    static Env_Struct& getInstance();

private:
    void init_parseConfig();
    void init_checkConfig(rapidjson::Document& doc);
    
    // Singleton
    Env_Struct() {};
    Env_Struct(Env_Struct const*);
    void operator=(Env_Struct const&);

}; // Class Env_Struct

}; // Namespace Config
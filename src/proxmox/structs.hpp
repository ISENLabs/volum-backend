#include <rapidjson/document.h>
#include <string>
#include <vector>
#include <map>

namespace Proxmox::Structs{

enum Proxmox_LXC_State {
    RUNNING,
    STOPPED
};

struct Proxmox_LXC { // LinuX Containers
    Proxmox_LXC_State status;
    std::string name;
    uint vm_id;
    ushort cpus; // Number of CPUs
    size_t maxmem; // Allowed RAM
    size_t maxdisk; // Allowed disk amount
    size_t maxswap; // Allowed SWAP

    uint uptime;
    double cpu; // CPU usage metric (%)
    size_t disk; // Disk usage
    size_t mem; // ram usage
    size_t swap; // swap usage
    size_t net_in;
    size_t net_out;

    std::string ip_address = ""; // empty by default!
    std::string subdomain = "";
    std::string password = ""; // empty by default!
    uint owner_id = 0;

    std::string owner_name = "";
    std::string owner_email = "";
    std::string owner_class = "";
};
typedef std::vector<Proxmox_LXC> Proxmox_LXCS;

class Converters {
public:
    static std::string lxc_to_json(Proxmox_LXC lxc);
    static std::string lxcs_to_json(Proxmox_LXCS lxcs);
    static Proxmox_LXC json_to_lxc(rapidjson::Value& json);
    static Proxmox_LXCS json_to_lxcs(rapidjson::Value& json);
};

};
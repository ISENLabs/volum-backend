#pragma once
#include "requests.hpp"
#include "structs.hpp"
#include "utils.hpp"
#include "../utils/cache.hpp"

#define PROXMOX_LXC_CACHE_TTL 15

using namespace Proxmox::Structs;
using namespace Utils::Cache;
namespace Proxmox{
class Methods{

private:
    static CacheHandler<uint, Proxmox_LXC>& get_cache();
public:
    static Proxmox_LXC get_lxc(uint pct_id); // returns basic informations + pv ipv4 address
    static Proxmox_LXCS get_lxcs();
    static std::string get_pv_ipv4(uint pct_id);
    static Proxmox_LXC create_lxc(); // TODO: Définir les arguments etc.

    static bool delete_lxc(uint pct_id);
    static bool stop_lxc(uint pct_id);
    static bool start_lxc(uint pct_id);
    
};
};
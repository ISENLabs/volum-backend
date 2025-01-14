#include "requests.hpp"
#include "structs.hpp"
#include "utils.hpp"

using namespace Proxmox::Structs;
namespace Proxmox{
class Methods{

private:

public:
    Proxmox_LXC get_lxc(uint pct_id);
    Proxmox_LXCS get_lxcs();
    Proxmox_LXC create_lxc(...); // TODO: Définir les arguments etc.

    bool delete_lxc(uint pct_id);
    bool stop_lxc(uint pct_id);
    bool start_lxc(uint pct_id);
};
};
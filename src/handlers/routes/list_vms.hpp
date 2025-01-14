#include "../../utils/debug.hpp"
#include "../middlewares/auth.hpp"
#include <crow.h>
#include <iostream>

namespace Handlers::Routes::VMS{
    std::string list_vms(Handlers::Middlewares::Auth::context& ctx){
        // Check if admin

        // Get Proxmox PCT list

        // Get Mariadb vms list

        // Aggregate, to also return the VM state + stats.
        return "{\"success\":true, \"message\":\"success\", \"data\":{...todo...}}";
    }

};
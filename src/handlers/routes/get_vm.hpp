#include "../../utils/debug.hpp"
#include "../../services/database.hpp"
#include "../../proxmox/methods.hpp"
#include "../middlewares/auth.hpp"
#include <set>
#include <crow.h>
#include <iostream>

using Services::Database;
using namespace Proxmox::Structs;

namespace Handlers::Routes::VMS{
    std::string get_vm(Handlers::Middlewares::Auth::context& ctx, uint pct_id){
        if(ctx.user.userId < 1){
            return "{\"success\":false, \"error\":\"Unauthenticated user\"";
        }

        // Get Mariadb vms list
        auto& db = Database::getInstance();
        auto& conn = db.getConnection();
        std::shared_ptr<sql::PreparedStatement> stmnt(conn->prepareStatement(
                  ctx.user.is_admin ? "SELECT * FROM volum_vms WHERE ctid = ? " : "SELECT * FROM volum_vms WHERE ctid = ? AND user_id = ?"
               )
            );
        
        stmnt->setInt(1, pct_id);
        if(!ctx.user.is_admin)
            stmnt->setInt(2, ctx.user.userId);
        auto *res = stmnt->executeQuery();

        try{
            if(res->next()){
                uint _pct_id = res->getInt(2);
                Proxmox_LXC lxc = Proxmox::Methods::get_lxc(_pct_id);
                // Add IP + subdomain
                lxc.ip_address = res->getString(3);
                lxc.subdomain = res->getString(5);
                return "{\"success\":true, \"message\":\"success\", \"data\":["+ Converters::lxc_to_json(lxc) +"]}";
            }
            else{
                return "{\"success\":false, \"error\":\"unknown vm\"";
            }
        }
        catch(std::exception& ex){
            Debug::Log("Error while getting vm: " + std::string(ex.what()), "R-GVM");
            return "{\"success\":false, \"error\":\"internal error\"";
        }
    }
};
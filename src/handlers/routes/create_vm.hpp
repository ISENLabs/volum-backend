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
    std::string create_vm(Handlers::Middlewares::Auth::context& ctx, uint pct_id){
        if(ctx.user.userId < 1){
            return "{\"success\":false, \"error\":\"Unauthenticated user\"}";
        }

        auto& db = Database::getInstance();
        auto& conn = db.getConnection();
        std::shared_ptr<sql::PreparedStatement> stmnt(conn->prepareStatement(
                  "SELECT ctid FROM volum_vms WHERE ctid = ? AND user_id = ?"
               )
            );

        stmnt->setInt(1, pct_id);
        stmnt->setInt(2, ctx.user.userId);
        auto *res = stmnt->executeQuery();

        try{
            // Check if user already has a vm
            if(!res->next() || ctx.user.is_admin){
                std::string subdomain = std::to_string(pct_id)+"-"+std::to_string(ctx.user.userId)+"-volum.isenlabs.fr";
                Proxmox_LXC lxc = Proxmox::Methods::create_lxc(pct_id, ctx.user.userId, subdomain.c_str());
                return "{\"success\":true, \"message\":\"success\", \"data\":["+ Converters::lxc_to_json(lxc) +"]}";
            }

            return "{\"success\":false, \"error\":\"Already have vm\"}";
        }
        catch(std::exception& ex){
            Debug::Log("Error while getting vm: " + std::string(ex.what()), "R-GVM");
            return "{\"success\":false, \"error\":\"internal error\"}";
        }
    }
};
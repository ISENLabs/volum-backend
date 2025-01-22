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
            return "{\"success\":false, \"error\":\"Unauthenticated user\"}";
        }

        auto& env = Utils::Config::Env_Struct::getInstance();
        if(pct_id < env.vm_startIndex || pct_id >= env.vm_startIndex + 256*256) // /16
            return "{\"success\":false, \"error\":\"api misuse?\"}";

        // Get Mariadb vms list
        auto& db = Database::getInstance();
        auto& conn = db.getConnection();
        std::shared_ptr<sql::PreparedStatement> stmnt(conn->prepareStatement(
            ctx.user.is_admin ? 
            "SELECT v.*, u.name as owner_name, u.email as owner_email, u.class_name as owner_class "
            "FROM volum_vms v "
            "JOIN volum_users u ON v.user_id = u.id "
            "WHERE v.ctid = ?" 
            : 
            "SELECT v.*, u.name as owner_name, u.email as owner_email, u.class_name as owner_class "
            "FROM volum_vms v "
            "JOIN volum_users u ON v.user_id = u.id "
            "WHERE v.ctid = ? AND v.user_id = ?"
        ));
        
        stmnt->setInt(1, pct_id);
        if(!ctx.user.is_admin)
            stmnt->setInt(2, ctx.user.userId);
        auto *res = stmnt->executeQuery();

        try{
            if(res->next()) {
                uint vm_id = res->getInt("ctid");
                Proxmox_LXC lxc = Proxmox::Methods::get_lxc(vm_id);
                
                // Add IP + subdomain + owner information
                lxc.owner_id = res->getUInt("user_id");
                lxc.ip_address = res->getString("internal_ip");
                lxc.subdomain = res->getString("subdomain");
                lxc.owner_name = res->getString("owner_name");
                lxc.owner_email = res->getString("owner_email");
                lxc.owner_class = res->getString("owner_class");

                return "{\"success\":true, \"message\":\"success\", \"data\":["+ Converters::lxc_to_json(lxc) +"]}";
            }
            else {
                return "{\"success\":false, \"error\":\"unknown vm\"}";
            }
        }
        catch(std::exception& ex){
            Debug::Log("Error while getting vm: " + std::string(ex.what()), "R-GVM");
            return "{\"success\":false, \"error\":\"internal error\"}";
        }
    }
};
#include "../../utils/debug.hpp"
#include "../../services/database.hpp"
#include "../../proxmox/methods.hpp"
#include "../middlewares/auth.hpp"
#include <set>
#include <crow.h>
#include <iostream>
#include <thread>
#include <chrono>

using Services::Database;
using namespace Proxmox::Structs;
using namespace std::this_thread;
using namespace std::chrono_literals;

namespace Handlers::Routes::VMS{
    std::string delete_vm(Handlers::Middlewares::Auth::context& ctx, uint pct_id){
        if(ctx.user.userId < 1){
            return "{\"success\":false, \"error\":\"Unauthenticated user\"}";
        }

        auto& env = Env_Struct::getInstance();
        if(pct_id < env.vm_startIndex || pct_id >= env.vm_startIndex + 256*256) // /16
            return "{\"success\":false, \"error\":\"api misuse?\"}";

        // Get Mariadb vms list
        auto& db = Database::getInstance();
        auto& conn = db.getConnection();
        std::shared_ptr<sql::PreparedStatement> stmnt(conn->prepareStatement(
                  ctx.user.is_admin ? "SELECT ctid FROM volum_vms WHERE ctid = ? " : "SELECT ctid FROM volum_vms WHERE ctid = ? AND user_id = ?"
               )
            );

        stmnt->setInt(1, pct_id);
        if(!ctx.user.is_admin) stmnt->setInt(2, ctx.user.userId);
        auto *res = stmnt->executeQuery();

        try{
            if(res->next()){
                Proxmox_LXC lxc = Proxmox::Methods::get_lxc(pct_id);
                if(lxc.status == Proxmox::Structs::Proxmox_LXC_State::RUNNING){
                    // Stop the vm
                    if(!Proxmox::Methods::stop_lxc(pct_id)){
                        return "{\"success\":\"false\", \"error\":\"Can't automatically stop the VM. Please stop it before deleting.\"}";
                    }
                    sleep_for(2s); // Wait for the actual ct shutdown
                }

                if(!Proxmox::Methods::delete_lxc(pct_id)) {
                    throw std::runtime_error("Error while deleting vm: " + std::to_string(pct_id));
                }

                std::shared_ptr<sql::PreparedStatement> stmnt(conn->prepareStatement(
                      "DELETE FROM volum_vms WHERE ctid= ? LIMIT 1;"
                   )
                );
                stmnt->setInt(1, pct_id);
                stmnt->executeQuery();

                return "{\"success\":true, \"message\":\"success\"}";
            } else{
                return "{\"success\":false, \"error\":\"unknown vm\"}";
            }
        }
        catch(std::exception& ex){
            Debug::Log("Error while getting vm: " + std::string(ex.what()), "R-DVM");
            return "{\"success\":false, \"error\":\"internal error\"}";
        }
    }
};
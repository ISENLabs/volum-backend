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
    std::string list_vms(Handlers::Middlewares::Auth::context& ctx){
        if(ctx.user.userId < 1){
            return "{\"success\":false, \"error\":\"Unauthenticated user\"";
        }

        // Get Mariadb vms list
        auto& db = Database::getInstance();
        auto& conn = db.getConnection();
        std::shared_ptr<sql::PreparedStatement> stmnt(conn->prepareStatement(
                  ctx.user.is_admin ? "SELECT * FROM volum_vms" : "SELECT * FROM volum_vms WHERE user_id = ?"
               )
            );
        if(!ctx.user.is_admin)
            stmnt->setInt(1, ctx.user.userId);
        auto *res = stmnt->executeQuery();

        std::set<uint> vm_ids;
        std::map<uint, std::string> vm_ips; // cache
        std::map<uint, std::string> vm_subdomains; // cache
        while(res->next()){
            vm_ids.insert(res->getInt(2)); // ctid
            vm_ips.insert({(uint)res->getInt(2), std::string(res->getString(3))});
            vm_subdomains.insert({(uint)res->getInt(2), std::string(res->getString(5))});
        }

        try{
            // Get Proxmox PCT list

            if(vm_ids.size() == 0){
                return "{\"success\":true, \"message\":\"success\", \"data\":[]}";
            }

            // For one VM
            if(vm_ids.size() == 1){
                Proxmox_LXC lxc = Proxmox::Methods::get_lxc(*(vm_ids.begin()));
                lxc.ip_address = vm_ips[lxc.vm_id];
                lxc.subdomain = vm_subdomains[lxc.vm_id];
                return "{\"success\":true, \"message\":\"success\", \"data\":["+ Converters::lxc_to_json(lxc) +"]}";
            }

            // Multiple VMs
            Proxmox_LXCS lxcs = Proxmox::Methods::get_lxcs();
            // delete unknown lxcs
            auto it = lxcs.begin();
            while(it != lxcs.end()){
                if(vm_ids.find(it->vm_id) == vm_ids.end())
                    it = lxcs.erase(it);
                else{
                    it->ip_address = vm_ips[it->vm_id];
                    it->subdomain = vm_subdomains[it->vm_id];
                    it++;
                }
            }

            std::string _json = Converters::lxcs_to_json(lxcs);

            return "{\"success\":true, \"message\":\"success\", \"data\":"+ _json +"}";
        }
        catch(std::exception& ex){
            Debug::Log("Unable to complete list_vms query: " + std::string(ex.what()), "R-LVM");
            return "{\"success\":false, \"message\":\"internal error\"";
        }
    }

};
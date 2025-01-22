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
            return "{\"success\":false, \"error\":\"Unauthenticated user\"}";
        }

        // Get Mariadb vms list
        auto& db = Database::getInstance();
        auto& conn = db.getConnection();
        std::shared_ptr<sql::PreparedStatement> stmnt(conn->prepareStatement(
            ctx.user.is_admin ? 
            "SELECT v.*, u.name as owner_name, u.email as owner_email, u.class_name as owner_class "
            "FROM volum_vms v "
            "JOIN volum_users u ON v.user_id = u.id "
            "ORDER BY v.ctid DESC" 
            : 
            "SELECT v.*, u.name as owner_name, u.email as owner_email, u.class_name as owner_class "
            "FROM volum_vms v "
            "JOIN volum_users u ON v.user_id = u.id "
            "WHERE v.user_id = ? "
            "ORDER BY v.ctid DESC"
        ));
        if(!ctx.user.is_admin)
            stmnt->setInt(1, ctx.user.userId);
        auto *res = stmnt->executeQuery();

        std::set<uint> vm_ids;
        std::map<uint, std::string> vm_ips; // cache
        std::map<uint, std::string> vm_subdomains; // cache
        std::map<uint, uint> vm_owners; // cache
        std::map<uint, std::string> owner_names; // cache
        std::map<uint, std::string> owner_emails; // cache
        std::map<uint, std::string> owner_classes; // cache

        while(res->next()){
            uint ctid = res->getInt("ctid");
            vm_ids.insert(ctid);
            vm_ips.insert({ctid, std::string(res->getString("internal_ip"))});
            vm_subdomains.insert({ctid, std::string(res->getString("subdomain"))});
            vm_owners.insert({ctid, res->getUInt("user_id")});
            owner_names.insert({ctid, std::string(res->getString("owner_name"))});
            owner_emails.insert({ctid, std::string(res->getString("owner_email"))});
            owner_classes.insert({ctid, std::string(res->getString("owner_class"))});
        }

        try{
            // Get Proxmox PCT list
            if(vm_ids.size() == 0){
                return "{\"success\":true, \"message\":\"success\", \"data\":[]}";
            }

            // For one VM
            if(vm_ids.size() == 1){
                Proxmox_LXC lxc = Proxmox::Methods::get_lxc(*vm_ids.begin());
                lxc.ip_address = vm_ips[lxc.vm_id];
                lxc.subdomain = vm_subdomains[lxc.vm_id];
                lxc.owner_id = vm_owners[lxc.vm_id];
                lxc.owner_name = owner_names[lxc.vm_id];
                lxc.owner_email = owner_emails[lxc.vm_id];
                lxc.owner_class = owner_classes[lxc.vm_id];
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
                    it->owner_id = vm_owners[it->vm_id];
                    it->owner_name = owner_names[it->vm_id];
                    it->owner_email = owner_emails[it->vm_id];
                    it->owner_class = owner_classes[it->vm_id];
                    it++;
                }
            }

            std::string _json = Converters::lxcs_to_json(lxcs);

            return "{\"success\":true, \"message\":\"success\", \"data\":"+ _json +"}";
        }
        catch(std::exception& ex){
            Debug::Log("Unable to complete list_vms query: " + std::string(ex.what()), "R-LVM");
            return "{\"success\":false, \"message\":\"internal error\"}";
        }
    }

};
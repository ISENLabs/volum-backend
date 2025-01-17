#include "../../utils/debug.hpp"
#include "../../services/database.hpp"
#include "../../proxmox/methods.hpp"
#include "../../utils/config.hpp"
#include "../middlewares/auth.hpp"
#include <set>
#include <crow.h>
#include <iostream>
#include <sstream>

using Services::Database;
using namespace Proxmox::Structs;
using namespace Utils::Config;

namespace Handlers::Routes::VMS{
    bool isSubdomainValid(std::string subdomain){
        
        static const std::set<const char*> prohibed_subdomains = {
            "login",
            "register",
            "admin",
            "panel"
        };

        for(auto _prohibed : prohibed_subdomains)
            if(subdomain.c_str() == _prohibed)
                return false;

        if(subdomain.length() < 3 || subdomain.length() > 40) 
            return false;

        for(int i = 1; i < subdomain.length(); i++){
            if(!std::isalnum(subdomain[i]) && subdomain[i] != '-') 
                return false;
        }

        auto& db = Database::getInstance();
        auto& conn = db.getConnection();
        std::shared_ptr<sql::PreparedStatement> stmnt(conn->prepareStatement(
                  "SELECT subdomain FROM volum_vms;"
               )
            );

        auto *res = stmnt->executeQuery();
        while(res->next()){
            // If domain already exists
            if(subdomain == res->getString("subdomain")) 
                return false;
        }

        return true;
    }

    std::string create_vm(Handlers::Middlewares::Auth::context& ctx, const std::string& server_name, const std::string& subdomain){
        if(ctx.user.userId < 1){
            return "{\"success\":false, \"error\":\"Unauthenticated user\"}";
        }

        if(!isSubdomainValid(subdomain)) 
            return "{\"success\":false, \"error\":\"invalid subdomain\"}";

        // Limit to 1 vm/user (except. admins)
        auto& db = Database::getInstance();
        auto& conn = db.getConnection();
        std::shared_ptr<sql::PreparedStatement> stmnt(conn->prepareStatement(
                  "SELECT COUNT(*) FROM volum_vms WHERE user_id = ?"
               )
            );
        stmnt->setInt(1, ctx.user.userId);
        auto *res = stmnt->executeQuery();

        if(res->next()){
            if(res->getInt(1) > 0){
                return "{\"success\":\"true\", \"error\":\"You already have a VM.\"}";
            }
        }

        std::shared_ptr<sql::PreparedStatement> stmnt2(conn->prepareStatement(
                  "SELECT * FROM volum_vms ORDER BY id DESC LIMIT 1;"
               )
            );

        auto *res2 = stmnt2->executeQuery();

        try{
            uint pct_id = 0;
            std::string internal_ip = "";

            if(res2->next()) {
                pct_id = res2->getInt("ctid")+1;
                internal_ip = res2->getString("internal_ip");

                std::stringstream ss(internal_ip);
                std::string byte;
                std::vector<int> bytes;

                while (std::getline(ss, byte, '.')) {
                    bytes.push_back(std::stoi(byte));
                }

                if(bytes[3] == 255) {
                    if(bytes[2] == 255) 
                        throw std::runtime_error("All ip taken");
                    bytes[2]++;
                } 
                else 
                    bytes[3]++;

                internal_ip = std::to_string(bytes[0]) + "." + std::to_string(bytes[1]) + "." + std::to_string(bytes[2]) + "." + std::to_string(bytes[3]);
            } 
            else {
                // First VM
                auto& config = Env_Struct::getInstance();
                pct_id = config.vm_startIndex;
                internal_ip = config.vm_startIp;
            }

            Proxmox_LXC lxc = Proxmox::Methods::create_lxc(pct_id, server_name, internal_ip);

            // Add to db
            std::shared_ptr<sql::PreparedStatement> stmnt(conn->prepareStatement(
                  "INSERT INTO volum_vms(ctid, internal_ip, user_id, subdomain) VALUES(?, ?, ?, ?);"
               )
            );

            stmnt->setInt(1, pct_id);
            stmnt->setString(2, internal_ip);
            stmnt->setInt(3, ctx.user.userId);
            stmnt->setString(4, subdomain);
            stmnt->executeQuery();

            return "{\"success\":true, \"message\":\"success\", \"data\":["+ Converters::lxc_to_json(lxc) +"]}";
        }
        catch(std::exception& ex){
            Debug::Log("Error while getting vm: " + std::string(ex.what()), "R-CVM");
            return "{\"success\":false, \"error\":\"internal error\"}";
        }
    }
};
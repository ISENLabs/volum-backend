#include "../../utils/debug.hpp"
#include "../../services/database.hpp"
#include "../../proxmox/methods.hpp"
#include "../../utils/config.hpp"
#include "../middlewares/auth.hpp"
#include <set>
#include <crow.h>
#include <iostream>
#include <sstream>
#define MAX_VMS 2

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

    // Check if valid hostname (alphanumeric + '-').
    bool isValidServerName(std::string server_name){
        if(server_name.length() < 3 || server_name.length() > 40) 
            return false;

        for(int i = 1; i < server_name.length(); i++){
            if(!std::isalnum(server_name[i]) && server_name[i] != '-') 
                return false;
        }

        return true;
    }

    std::string create_vm(Handlers::Middlewares::Auth::context& ctx, const std::string& server_name, const std::string& subdomain){
        if(ctx.user.userId < 1){
            return "{\"success\":false, \"error\":\"Unauthenticated user\"}";
        }

        if(!isValidServerName(server_name))
            return "{\"success\":false, \"error\":\"invalid server name.\"}";

        if(!isSubdomainValid(subdomain)) 
            return "{\"success\":false, \"error\":\"invalid subdomain\"}";

        // Limit to MAX_VMS vm/user (except. admins)
        auto& db = Database::getInstance();
        auto& conn = db.getConnection();
        std::shared_ptr<sql::PreparedStatement> stmnt(conn->prepareStatement(
                  "SELECT COUNT(*) FROM volum_vms WHERE user_id = ?"
               )
            );
        stmnt->setInt(1, ctx.user.userId);
        auto *res = stmnt->executeQuery();

        int vm_count = 0;
        if(res->next()){
            vm_count = res->getInt(1);
        }

        if(vm_count >= MAX_VMS && !ctx.user.is_admin){
            return "{\"success\":false, \"error\":\"You reached VMs limit.\"}";
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
                    bytes[3] = 0;
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

            // Generate password
            std::string password = "";
            char valid_chars[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789._-";
            srand(time(NULL));

            for(int i = 0; i < 12; i++) 
                password += valid_chars[rand() % strlen(valid_chars)];

            Proxmox_LXC lxc = Proxmox::Methods::create_lxc(pct_id, server_name, internal_ip, password);
            lxc.password = password; // so it will be displayed to the user
            lxc.subdomain = subdomain;
            lxc.owner_id = ctx.user.userId;

            // check if the user is not in db
            std::shared_ptr<sql::PreparedStatement> stmnt3(conn->prepareStatement(
              "SELECT * FROM volum_users WHERE id = ?;"
               )
            );

            stmnt3->setInt(1, ctx.user.userId);
            auto *res3 = stmnt3->executeQuery();

            if(!res3->next()){
                // first add the user to db
                std::shared_ptr<sql::PreparedStatement> stmnt4(conn->prepareStatement(
                    "INSERT INTO volum_users(id, name, email, class_name) VALUES(?, ?, ?, ?);"
                )
                );

                stmnt4->setInt(1, ctx.user.userId);
                stmnt4->setString(2, ctx.user.name);
                stmnt4->setString(3, ctx.user.email);
                stmnt4->setString(4, ctx.user.class_name);
                stmnt4->executeQuery();
            }

            // Add to db
            std::shared_ptr<sql::PreparedStatement> stmnt5(conn->prepareStatement(
              "INSERT INTO volum_vms(ctid, internal_ip, user_id, subdomain) VALUES(?, ?, ?, ?);"
               )
            );

            stmnt5->setInt(1, pct_id);
            stmnt5->setString(2, internal_ip);
            stmnt5->setInt(3, ctx.user.userId);
            stmnt5->setString(4, subdomain);
            stmnt5->executeQuery();

            return "{\"success\":true, \"message\":\"success\", \"data\":["+ Converters::lxc_to_json(lxc) +"]}";
        }
        catch(std::exception& ex){
            Debug::Log("Error while getting vm: " + std::string(ex.what()), "R-CVM");
            return "{\"success\":false, \"error\":\"internal error\"}";
        }
    }
};
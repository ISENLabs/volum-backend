#pragma once
#include <iostream>
#include <crow.h>
#include <chrono>
#include <fstream>
#include "../../utils/debug.hpp"
#include "../../utils/config.hpp"
#include "../../providers/lazy_auth.hpp"

using Utils::Debug;

namespace Handlers::Middlewares{

struct Auth : crow::ILocalMiddleware{
    struct context {
        Services::Auth::User user;
    };

    void before_handle(crow::request& req, crow::response& res, context& ctx) {
        Utils::Config::Env_Struct& env = Utils::Config::Env_Struct::getInstance();
        auto token = req.get_header_value("auth-token");

        // Check for the cookie
        if (token.length() < 1) {
            Debug::Log("Request: " + crow::method_name(req.method) + " " + req.url + " (" + req.remote_ip_address + ") : No auth cookie", "HTTP-AUTH");
            res.code = 403;
            res.write("{\"success\":false, \"error\":\"Unauthorized\"}");
            res.end();
            return;
        }
        // Call the auth provider
        try{
            ctx.user = env.get_auth_provider()->checkAuth(token);
        }
        catch(std::exception& ex){
            // Strip the quotes so it wont break the json
            std::string stripped;
            std::string ex_what(ex.what());
            for (char c : ex_what) {
                if (c == '"') {
                    stripped += "\\\"";
                } else {
                    stripped += c;
                }
            }
            res.code = 403;
            res.write("{\"success\":false, \"message\":\"Unauthorized\", \"data\":{\"error_msg\":\""+ stripped +"\"}}");
            res.end();
            return;
        }
    }

    void after_handle(crow::request& req, crow::response& res, context& ctx) {}
};

};
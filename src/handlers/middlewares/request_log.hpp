#include <iostream>
#include <crow.h>
#include <chrono>
#include <fstream>
#include "../../utils/debug.hpp"

using Utils::Debug;

namespace Handlers::Middlewares{

struct RequestLogger{
    struct context {}; // We don't care about it

    void before_handle(crow::request& req, crow::response& res, context& ctx) {
        Debug::Log("Request: " + crow::method_name(req.method) + " " + req.url + " (" + req.remote_ip_address + ")", "HTTP");
    }

    void after_handle(crow::request& req, crow::response& res, context& ctx) {}

};

};
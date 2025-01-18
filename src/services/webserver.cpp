#include "../handlers/routes/defaults.hpp"
#include "../handlers/routes/user.hpp"
#include "../handlers/routes/list_vms.hpp"
#include "../handlers/routes/get_vm.hpp"
#include "../handlers/routes/create_vm.hpp"
#include "../handlers/routes/delete_vm.hpp"
#include "../handlers/routes/start_vm.hpp"
#include "../handlers/routes/stop_vm.hpp"
#include "webserver.hpp"

using Services::WebServer;
using namespace Handlers::Middlewares;
using namespace Handlers::Routes;

WebServer::WebServer(){
    crow::App<Handlers::Middlewares::Auth, RequestLogger> app;
}

WebServer::~WebServer(){
    app.stop();
}

void WebServer::register_routes(){
    //Register home (/) + 404
    CROW_CATCHALL_ROUTE(app)(Defaults::handle_notfounrd);
    CROW_ROUTE(app, "/")
        ([]() -> std::string
        { return Defaults::default_route(); });

    // Example route for auth testing
    CROW_ROUTE(app, "/user")
        .CROW_MIDDLEWARES(app, Handlers::Middlewares::Auth)
        ([&](const crow::request &req) {
            auto& ctx = app.get_context<Handlers::Middlewares::Auth>(req);
            return User_Routes::infos(ctx);
        });

    // List VMs
    CROW_ROUTE(app, "/vms")
        .CROW_MIDDLEWARES(app, Handlers::Middlewares::Auth)
        ([&](const crow::request &req) {
            auto& ctx = app.get_context<Handlers::Middlewares::Auth>(req);
            return VMS::list_vms(ctx);
        });

    // Get or Delete ONE vm details
    CROW_ROUTE(app, "/vms/<int>")
        .methods("GET"_method, "DELETE"_method)
        .CROW_MIDDLEWARES(app, Handlers::Middlewares::Auth)
        ([&](const crow::request &req, uint pct_id) {
            auto& ctx = app.get_context<Handlers::Middlewares::Auth>(req);
            return req.method == "GET"_method ? VMS::get_vm(ctx, pct_id) : VMS::delete_vm(ctx, pct_id);
        });

    // Start ONE vm
    CROW_ROUTE(app, "/vms/<int>/stop")
        .methods("POST"_method)
        .CROW_MIDDLEWARES(app, Handlers::Middlewares::Auth)
        ([&](const crow::request &req, uint pct_id) {
            auto& ctx = app.get_context<Handlers::Middlewares::Auth>(req);
            return VMS::stop_vm(ctx, pct_id);
        });

    // Stop ONE vm
    CROW_ROUTE(app, "/vms/<int>/start")
        .methods("POST"_method)
        .CROW_MIDDLEWARES(app, Handlers::Middlewares::Auth)
        ([&](const crow::request &req, uint pct_id) {
            auto& ctx = app.get_context<Handlers::Middlewares::Auth>(req);
            return VMS::start_vm(ctx, pct_id);
        });

    // Create ONE vm
    CROW_ROUTE(app, "/vms/create")
        .methods("POST"_method)
        .CROW_MIDDLEWARES(app, Handlers::Middlewares::Auth)
        ([&](const crow::request &req) {
            auto body = crow::json::load(req.body);

            if (!body || !body.has("server_name") || !body.has("subdomain"))
                return crow::response("{\"success\":false, \"error\":\"Missing server_name or subdomain\"}");

            std::string server_name = body["server_name"].s();
            std::string subdomain = body["subdomain"].s();

            auto& ctx = app.get_context<Handlers::Middlewares::Auth>(req);
            return crow::response(VMS::create_vm(ctx, server_name, subdomain));
        });
}

void WebServer::run_server(uint port){
    app.loglevel(crow::LogLevel::WARNING);

    app.port(port);
    app.multithreaded();
    app.run();
}
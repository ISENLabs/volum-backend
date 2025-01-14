#include "../handlers/routes/defaults.hpp"
#include "../handlers/routes/user.hpp"
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

}

void WebServer::run_server(uint port){
    app.loglevel(crow::LogLevel::WARNING);

    app.port(port);
    app.multithreaded();
    app.run();
}
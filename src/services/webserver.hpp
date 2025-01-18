#include "../handlers/middlewares/request_log.hpp"
#include "../handlers/middlewares/auth.hpp"
#include <iostream>
#include <memory>
#include <crow.h>
#include <crow/middlewares/cors.h>

namespace Services{
class WebServer{
private:
    crow::App<crow::CORSHandler, Handlers::Middlewares::Auth, Handlers::Middlewares::RequestLogger> app;

public:
    WebServer();
    ~WebServer();

    void register_routes();
    void run_server(uint port);

};
};
#include <iostream>
#include "src/utils/debug.hpp"
#include "src/utils/config.hpp"
#include "src/utils/cache.hpp"
#include "src/services/database.hpp"
#include "src/services/auth.hpp"
#include "src/services/webserver.hpp"
#include "src/providers/lazy_auth.hpp"
#include "src/utils/http.hpp"

using namespace Utils::Cache;
using Utils::Debug;
using Utils::Config::Env_Struct;
using Services::Database;


int main(){
    Debug::Log("Volum-Backend starting up...", "MAIN");
    // First thing to do: Parse config
    Debug::Log("Parsing env file", "MAIN");
    // Parse config
    Env_Struct& env = Env_Struct::getInstance();

    // Connect to DB
    Debug::Log("Connecting to DB", "MAIN");
    auto& db = Database::getInstance();
    // Test: retrieve status
    auto conn = db.getConnection();
    auto stmt = conn->createStatement();
    auto res = stmt->executeQuery("SELECT * FROM volum_status WHERE id = 1");
    while(res->next()){
        Debug::Log("Status: " + std::string(res->getString("status")) + " and Version: " + std::string(res->getString("version")), "MAIN");
    }

    // Change your auth provider here, if needed !
    Providers::Auth::Lazy::LazyAuth auth_provider;
    std::shared_ptr<Providers::Auth::Lazy::LazyAuth> provider = std::make_shared<Providers::Auth::Lazy::LazyAuth>(auth_provider);
    env.set_auth_handler(provider);

    // // test http
    // Utils::Http::Client client("https://api.sampleapis.com");
    // auto resp = client.get("/coffee/hot");
    // Debug::Log("Status: " + std::to_string(resp.status) + " " + resp.body, "MAIN");


    Debug::Log("Launching web server ...", "MAIN");
    Services::WebServer ws;

    CacheHandler<int, std::string> _ch(10);

    ws.register_routes();
    ws.run_server(env.webserver_port);

    return 0;
}
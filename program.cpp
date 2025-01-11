#include <iostream>
#include "src/utils/debug.hpp"
#include "src/utils/config.hpp"
#include "src/services/database.hpp"

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

    return 0;
}
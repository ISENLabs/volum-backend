#include "database.hpp"
#include "../utils/config.hpp"
#include "../utils/debug.hpp"

using Services::Database;
using Utils::Debug;
using Utils::Config::Env_Struct;

Database& Database::getInstance(){
    static Database instance;
    if(!instance.initialized){
        try{
            instance.connect();
            instance.initialized = true;
        }
        catch(const std::exception& e){
            Utils::Debug::Log("Unable to connect to DB: "+ std::string(e.what()), "DB-GETINST");
            exit(1);
        }
    }
    return instance;
}

void Database::connect(){
    auto& config = Env_Struct::getInstance();
    try{
        driver = sql::mariadb::get_driver_instance();
        connection = std::unique_ptr<sql::Connection>(driver->connect(config.db_host, config.db_user, config.db_password));
        connection->setSchema(config.db_name);
    }
    catch(const std::exception& e){
        Utils::Debug::Log("Unable to connect to DB: "+ std::string(e.what()), "DB-CONN");
    }
}

std::unique_ptr<sql::Connection>& Database::getConnection(){
    return connection;
}
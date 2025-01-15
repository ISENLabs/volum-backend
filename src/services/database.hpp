#pragma once
#include <iostream>
#include <mariadb/conncpp.hpp>

namespace Services{

class Database{
public:
    bool initialized = false;
    sql::Driver* driver;
    std::unique_ptr<sql::Connection> connection;

    // Singleton
    static Database& getInstance();

    // Methods
    std::unique_ptr<sql::Connection>& getConnection();
private:
    // Singleton
    Database() {};
    Database(Database const*);
    void operator=(Database const&);

    // Methods
    void connect();

}; // End of Database

};
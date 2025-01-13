#pragma once
#include <iostream>
#include <string>

namespace Services::Auth{

struct User {
    std::string firstname;
    std::string lastname;
    std::string email;
    uint userId;
};

class IAuthentication{

public:
    IAuthentication();
    ~IAuthentication();

    virtual User checkAuth(std::string token) = 0;
};


};
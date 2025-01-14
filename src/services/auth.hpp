#pragma once
#include <iostream>
#include <string>

namespace Services::Auth{

struct User {
    std::string firstname;
    std::string lastname;
    std::string email;
    uint userId;
    bool is_admin = false;

    std::string inline to_json(){
        return "{"
            "\"firstname\":\"" + firstname + "\","
            "\"lastname\":\"" + lastname + "\","
            "\"email\":\"" + email + "\"," 
            "\"id\":" + std::to_string(userId) +
        "}";
    }
};

class IAuthentication{

public:
    IAuthentication(){};
    ~IAuthentication(){};

    virtual User checkAuth(std::string token) = 0;
};


};
#pragma once
#include <iostream>
#include <string>

namespace Services::Auth{

struct User {
    // std::string firstname;
    // std::string lastname;
    std::string name;
    std::string email;
    std::string class_name;
    uint userId = 0;
    bool is_admin = false;

    std::string inline to_json(){
        return "{"
            // "\"firstname\":\"" + firstname + "\","
            // "\"lastname\":\"" + lastname + "\","
            "\"name\":\"" + name + "\","
            "\"email\":\"" + email + "\"," 
            "\"id\":" + std::to_string(userId) + ","
            "\"is_admin\":" + (is_admin ? "true":"false") + "\","
            "\"class_name\":" + class_name + "\"" +
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
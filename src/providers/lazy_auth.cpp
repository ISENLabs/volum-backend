#include "lazy_auth.hpp"
#include "../utils/config.hpp"
#include "../utils/debug.hpp"
#include "../utils/http.hpp"
#include <rapidjson/document.h>
#include <iostream>
#include <exception>

using Utils::Debug;
using Utils::Config::Env_Struct;
using Utils::Http::Client;
using Utils::Http::Response;
using Services::Auth::User;
using Providers::Auth::Lazy::LazyAuth;
using rapidjson::Document;
using rapidjson::Value;

User LazyAuth::checkAuth(std::string token){
    std::string json = makeLazyRequest(token);
    Document doc;
    doc.Parse(json.c_str());

    if(doc.HasParseError()){
        Debug::Log("Error while parsing json", "LA-CA");
        throw std::runtime_error("Error while authenticating you");
    }

    // First, check if the user is marked as admin
    if(!doc.HasMember("status") || (doc["status"].IsUint() && doc["status"].GetUint() != 200)){
        Debug::Log("Error in reading json: $.status step", "LA-CA");
        throw std::runtime_error("Error while authenticating you");
    }

    if(!doc.HasMember("info")){
        Debug::Log("Error while reading json: $.info does not exists", "LA-CA");
        throw std::runtime_error("Error while authenticating you");
    }
    
    Value& info = doc["info"];

    // Extract user infos
    User user;

    if(info.HasMember("firstname") && info["firstname"].IsString()){
        user.firstname = std::string(info["firstname"].GetString());
    }
    else{
        Debug::Log("Error while reading json: no $.info.firstname found", "LA-CA");
        throw new std::runtime_error("Error while authenticating you");
    }

    if(info.HasMember("lastname") && info["lastname"].IsString()){
        user.lastname = std::string(info["lastname"].GetString());
    }
    else{
        Debug::Log("Error while reading json: no $.info.lastname found", "LA-CA");
        throw new std::runtime_error("Error while authenticating you");
    }

    if(info.HasMember("email") && info["email"].IsString()){
        user.email = std::string(info["email"].GetString());
    }
    else{
        Debug::Log("Error while reading json: no $.info.email found", "LA-CA");
        throw new std::runtime_error("Error while authenticating you");
    }

    if(info.HasMember("user_id") && info["user_id"].IsUint()){
        user.userId = info["user_id"].GetUint();
    }
    else{
        Debug::Log("Error while reading json: no $.info.user_id found", "LA-CA");
        throw new std::runtime_error("Error while authenticating you");
    }

    // Check if the user is admin
    bool is_admin = info.HasMember("is_admin") && info["is_admin"].IsUint() && info["is_admin"].GetUint() == 1;

    if(!is_admin && info.HasMember("is_banned") && info["is_banned"].IsUint() && info["is_banned"].GetUint() ==1){
        Debug::Log("Error: user is banned", "LA-CA");
        throw std::runtime_error("Error while authenticatin you: banned");
    }

    // Only authorize Caen campus, for the moment.
    if(info.HasMember("class_name") && info["class_name"].IsString()){
        std::string classname(info["class_name"].GetString());

        // look for the last " "
        // CIR 3 CAEN
        int last_space = 0;
        int _len = classname.length();
        for(int i = 0; i < _len; i++)
            if(classname[i] == ' ')
                last_space = i;

        if(classname.substr(_len - (_len - 1  - last_space)) != "Caen"){
            Debug::Log("Error: class isnt caen: '" + classname + "' -> '" + classname.substr(_len - (_len - 1 - last_space)) + "'", "LA-CA");
            throw std::runtime_error("Error: Volum is only available for Caen campus right now.");
        }
    }
    else{
        Debug::Log("Error while reading json: no $.info.class_name ", "LA-CA");
        throw std::runtime_error("Error while authenticatin you");
    }
    
    return user;
}

/// @brief Make the requests to Lazy and extract the body
/// @param jwt 
/// @return JSON response
std::string LazyAuth::makeLazyRequest(std::string auth_token){
    // Get env instance
    Env_Struct& env = Env_Struct::getInstance();

    // Make the http request
    Client client(env.auth_lazyHostname.c_str());
    client.set_header("Cookie", std::string("paresseux-auth-token=" + auth_token).c_str());
    Response resp = client.get(env.auth_lazyRoute.c_str());

    // Check if success
    if(resp.error){
        Debug::Log("Erorr while querying lazy: " + resp.error_reason, "LA-MLR");
        throw std::runtime_error("Error while authenticating you");
    }

    if(resp.status != 200){
        Debug::Log("Error while querying lazy: status code is " + std::to_string(resp.status), "LA-MLR");
        throw std::runtime_error("Error while authenticating you");
    }

    return resp.body;
}


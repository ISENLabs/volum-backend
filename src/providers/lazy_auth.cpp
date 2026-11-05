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
    // Check cache
    auto& _cache = getAuthCache();
    auto entry = _cache.get_element(token);
    if(entry.has_value()){
        return *entry;
    }

    std::string json = makeLazyRequest(token);
    Document doc;
    doc.Parse(json.c_str());

    if(doc.HasParseError()){
        Debug::Log("Error while parsing json", "LA-CA");
        throw std::runtime_error("Error while authenticating you");
    }

    // Lazy v2 format: GET /user/info -> {"infos": {...}, "notif": [...]}
    if(!doc.IsObject() || !doc.HasMember("infos") || !doc["infos"].IsObject()){
        Debug::Log("Error while reading json: $.infos does not exists", "LA-CA");
        throw std::runtime_error("Error while authenticating you");
    }

    Value& info = doc["infos"];

    // Extract user infos
    User user;

    if(info.HasMember("name") && info["name"].IsString()){
        user.name = std::string(info["name"].GetString());
    }
    else{
        Debug::Log("Error while reading json: no $.infos.name found", "LA-CA");
        throw std::runtime_error("Error while authenticating you");
    }

    if(info.HasMember("email") && info["email"].IsString()){
        user.email = std::string(info["email"].GetString());
    }
    else{
        Debug::Log("Error while reading json: no $.infos.email found", "LA-CA");
        throw std::runtime_error("Error while authenticating you");
    }

    if(info.HasMember("id") && info["id"].IsUint()){
        user.userId = info["id"].GetUint();
    }
    else{
        Debug::Log("Error while reading json: no $.infos.id found", "LA-CA");
        throw std::runtime_error("Error while authenticating you");
    }

    // class_name = "<cycle> <year> <city>" (same as Lazy v1)
    if(info.HasMember("cycle") && info["cycle"].IsString()
        && info.HasMember("year") && info["year"].IsString()
        && info.HasMember("city") && info["city"].IsString()){
        user.class_name = std::string(info["cycle"].GetString()) + " "
            + info["year"].GetString() + " "
            + info["city"].GetString();
    }
    else{
        Debug::Log("Error while reading json: no $.infos.cycle/year/city found", "LA-CA");
        throw std::runtime_error("Error while authenticating you");
    }

    // Check if the user is admin
    bool is_admin = info.HasMember("is_admin") && info["is_admin"].IsBool() && info["is_admin"].GetBool();
    user.is_admin = is_admin;

    bool is_banned = info.HasMember("is_banned") && info["is_banned"].IsBool() && info["is_banned"].GetBool();
    if(!is_admin && is_banned){
        Debug::Log("Error: user is banned", "LA-CA");
        throw std::runtime_error("Error while authenticatin you: banned");
    }

    // Store user in cache
    _cache.store_element(token, user);
    
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

Utils::Cache::CacheHandler<std::string, User>& LazyAuth::getAuthCache() {
    static Utils::Cache::CacheHandler<std::string, User> cache_inst(LAZY_AUTH_CACHE_TTL);
    return cache_inst;
}

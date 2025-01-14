#include "http.hpp"
#include <openssl/ssl.h>
#include <rapidjson/document.h>

using Utils::Http::Client;
using Utils::Http::Response;

/// @brief Create a Client object
/// @param host example: "https://api.example.com"
/// @param json Default true. If true, it will add "Content-Type: application/json" to the headers
Client::Client(const char* host, bool json): json(json){
    host = host;
    client = new httplib::Client(host);
    set_cert_validation(false); // By default
}

Client::~Client(){
    delete client;
}

void Client::set_header(const char* key, const char* value){
    headers[key] = value;
}

void Client::set_timeout(uint timeout){
    this->timeout = timeout;
}

/// @brief Make a GET request
/// @param path example: "/api/v1/resource"
/// @return Response object
Response Client::get(const char* path){
    httplib::Headers headers;

    for(auto& [key, value] : this->headers){
        headers.insert({key, value});
    }

    client->set_connection_timeout(timeout);
    Response response;
    if(auto res = client->Get(path, headers)){
        response.status = res->status;
        response.body = res->body;
        response.error = false;
        response.error_reason = "_none";
    }
    else{
        response.error = true;
        response.error_reason = httplib::to_string(res.error());
    }
    return response;
}

/// @brief Make a POST request
/// @param path example: "/api/v1/ressource"
/// @param body Body that will be sent.
/// @return Response object
Response Client::post(const char* path, const char* body){
    httplib::Headers headers;

    for(auto& [key, value] : this->headers){
        headers.insert({key, value});
    }

    client->set_connection_timeout(timeout);
    Response response;
    if(auto res = client->Post(path, headers, body, json ? "application/json" : "multipart/form-data")){
        response.status = res->status;
        response.body = res->body;
        response.error = false;
        response.error_reason = "_none";
    }
    else{
        response.error = true;
        response.error_reason = httplib::to_string(res.error());
    }
    return response;
}

/// @brief Make a PUT request
/// @param path example: "/api/v1/ressource"
/// @param body Body that will be sent.
/// @return Response object
Response Client::put(const char* path, const char* body){
    httplib::Headers headers;

    for(auto& [key, value] : this->headers){
        headers.insert({key, value});
    }

    client->set_connection_timeout(timeout);
    Response response;
    if(auto res = client->Put(path, headers, body, json ? "application/json" : "multipart/form-data")){
        response.status = res->status;
        response.body = res->body;
        response.error = false;
        response.error_reason = "_none";
    }
    else{
        response.error = true;
        response.error_reason = httplib::to_string(res.error());
    }
    return response;
}

/// @brief Make a DELETE request
/// @param path example: "/api/v1/ressource"
/// @param body Body that will be sent.
/// @return Response object
Response Client::del(const char* path, const char* body){
    httplib::Headers headers;

    for(auto& [key, value] : this->headers){
        headers.insert({key, value});
    }

    client->set_connection_timeout(timeout);
    Response response;
    if(auto res = client->Delete(path, headers, body, json ? "application/json" : "multipart/form-data")){
        response.status = res->status;
        response.body = res->body;
        response.error = false;
        response.error_reason = "_none";
    }
    else{
        response.error = true;
        response.error_reason = httplib::to_string(res.error());
    }
    return response;
}

void Client::set_cert_validation(bool validate){
    validate_cert = validate;
    client->enable_server_certificate_verification(validate);
}
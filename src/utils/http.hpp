#define CPPHTTPLIB_OPENSSL_SUPPORT
#include <httplib.h>
#include <iostream>

namespace Utils::Http{

struct Response{
    int status;
    std::string body;
    bool error = false;
    std::string error_reason;
};

class Client{

public:
    Client(const char* host, bool json = true);
    ~Client();
    void set_header(const char* key, const char* value);
    void set_timeout(uint timeout);
    void set_cert_validation(bool validate);

    Response get(const char* path);
    Response post(const char* path, const char* body);
    Response put(const char* path, const char* body);
    Response del(const char* path, const char* body);

private:
    httplib::Client* client;
    const char* host;
    bool json;
    std::map<std::string, std::string> headers; // Default headers: none
    uint timeout = 5;
    bool validate_cert;

}; // Client class

}; // Namespace
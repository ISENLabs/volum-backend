#pragma once
#include "../services/auth.hpp"
#include "../utils/cache.hpp"

#define LAZY_AUTH_CACHE_TTL 120 // 2mn

using Services::Auth::IAuthentication;
using Services::Auth::User;

namespace Providers::Auth::Lazy {

class LazyAuth : public IAuthentication {

private: 
    std::string makeLazyRequest(std::string token);
    static Utils::Cache::CacheHandler<std::string, User>& getAuthCache();

public:
    LazyAuth(){};
    ~LazyAuth(){};
    User checkAuth(std::string token) override;
};

};
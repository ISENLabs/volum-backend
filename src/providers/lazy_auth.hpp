#pragma once
#include "../services/auth.hpp"


using Services::Auth::IAuthentication;
using Services::Auth::User;

namespace Providers::Auth::Lazy {

class LazyAuth : public IAuthentication {

private: 
    std::string makeLazyRequest(std::string token);

public:
    User checkAuth(std::string token) override;
};

};
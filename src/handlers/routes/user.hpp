#include "../../utils/debug.hpp"
#include "../middlewares/auth.hpp"
#include <crow.h>
#include <iostream>

namespace Handlers::Routes::User_Routes{

    std::string infos(Handlers::Middlewares::Auth::context& ctx){
        return "{"
                    "\"success\":true,"
                    "\"message\":\"Success, connected.\","
                    "\"data\":" + ctx.user.to_json() +
                "}";
    }

};
#include "../../utils/debug.hpp"
#include "../middlewares/auth.hpp"
#include <crow.h>
#include <iostream>

namespace Handlers::Routes::Defaults{
    std::string default_route(){
        return "{\"success\":true, \"message\":\"Online !\"}";
    }

    std::string handle_notfounrd(){
        return "{\"success\":false, \"error\":\"not found\"}";
    }
};
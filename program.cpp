#include <iostream>
#include "src/utils/debug.hpp"
#include "src/utils/config.hpp"

using Utils::Debug;
using Utils::Config::Env_Struct;

int main(){
    Debug::Log("Volum-Backend starting up...", "MAIN");
    // First thing to do: Parse config
    Debug::Log("Parsing env file", "MAIN");
    // Parse config
    Env_Struct& env = Env_Struct::getInstance();

    return 0;
}
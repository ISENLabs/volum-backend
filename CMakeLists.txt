cmake_minimum_required(VERSION 3.14)
set(CMAKE_CXX_STANDARD 20)

project(
    "volum-backend"
    VERSION 1.0
    DESCRIPTION "Volum Backend"
    LANGUAGES CXX
)

# Pour debug
set(CMAKE_BUILD_TYPE Debug)

# Lib externes
include_directories(exts)

add_executable(
    volum-backend
    program.cpp
    src/utils/config.cpp
    src/utils/http.cpp
    src/providers/lazy_auth.cpp
    src/services/database.cpp
    src/proxmox/methods.cpp
    src/proxmox/requests.cpp
    src/proxmox/structs.cpp
    src/services/webserver.cpp
)

target_compile_options(
    volum-backend
    PUBLIC
        -O0
        -g
)

target_link_options(
    volum-backend
    PUBLIC
)

# Libs
find_package(OpenSSL 3.0 REQUIRED)
find_package(httplib REQUIRED)
find_package(unofficial-mariadb-connector-cpp CONFIG REQUIRED)

# Static link
set(Boost_USE_STATIC_LIBS ON)
set(BUILD_SHARED_LIBS OFF)

target_link_libraries(
    volum-backend
    PRIVATE
    httplib::httplib
    OpenSSL::SSL
    OpenSSL::Crypto
    unofficial::mariadb-connector-cpp::mariadbcpp
)

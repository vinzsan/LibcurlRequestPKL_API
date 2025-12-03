cmake_minimum_required(VERSION 3.10)

project(cpp_rs)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(Test main.cpp)
add_compile_options(-fsanitized=address -g)
find_package(nlohmann_json CONFIG REQUIRED)
target_link_libraries(Test PRIVATE curl raylib nlohmann_json::nlohmann_json)
# CMake 最低版本号要求
cmake_minimum_required (VERSION 3.24)
# 项目信息

file(GLOB_RECURSE PROJECT_HEADERS *.h)
file(GLOB_RECURSE PROJECT_CPPS *.cpp)


set(ALL_FILES ${PROJECT_HEADERS} ${PROJECT_CPPS})

auto_source_group(game_test "${ALL_FILES}" DST_FILES "")

add_executable(game_test ${DST_FILES})

target_link_libraries(game_test PRIVATE python311_d bcrypt imgui jsoncpp_lib jsoncpp_object JsonCpp::JsonCpp Eigen3::Eigen Tracy::TracyClient)


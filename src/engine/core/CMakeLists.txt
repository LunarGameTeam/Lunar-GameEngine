# CMake 最低版本号要求
cmake_minimum_required (VERSION 2.8)
# 项目信息

file(GLOB_RECURSE PROJECT_HEADERS *.h)
file(GLOB_RECURSE PROJECT_CPPS *.cpp)

set(ALL_FILES ${PROJECT_HEADERS} ${PROJECT_CPPS})
auto_source_group("${ALL_FILES}" DST_FILES)

add_library(core SHARED ${DST_FILES})
add_compile_definitions(CORE_EXPORT)

add_dependencies(core imgui jsoncpp_static)
target_link_libraries(core imgui)


# CMake 最低版本号要求
cmake_minimum_required (VERSION 2.8)
# 项目信息

set(ALL_FILES engine.h engine.cpp)

auto_source_group(${ALL_FILES})

add_library(engine STATIC ${ALL_FILES})

add_dependencies(engine render window)

target_link_libraries(engine render window)


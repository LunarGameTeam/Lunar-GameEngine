# CMake 最低版本号要求
cmake_minimum_required (VERSION 2.8)
# 项目信息

file(GLOB_RECURSE PROJECT_HEADERS *.h)
file(GLOB_RECURSE PROJECT_CPPS *.cpp)

set(ALL_FILES ${PROJECT_HEADERS} ${PROJECT_CPPS})

auto_source_group("${ALL_FILES}" DST_FILES)

add_compile_definitions(IMGUI_EXPORT)

include_directories(${SDK_PATH}sdl/include)
auto_source_group("${ALL_FILES}" DST_FILES)

add_library(imgui SHARED ${DST_FILES})

add_dependencies(imgui SDL2)
target_link_libraries(imgui SDL2)


# CMake 最低版本号要求
cmake_minimum_required (VERSION 3.24)

file(GLOB_RECURSE PROJECT_HEADERS *.h)
file(GLOB_RECURSE PROJECT_CPPS *.cpp)

set(ALL_FILES ${PROJECT_HEADERS} ${PROJECT_CPPS})
auto_source_group("${ALL_FILES}" DST_FILES)

add_library(game SHARED ${DST_FILES})

add_compile_definitions(GAME_EXPORT)

add_dependencies(game core window render) 
target_link_libraries(game core window render)

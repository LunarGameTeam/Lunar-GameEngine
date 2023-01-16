# CMake 最低版本号要求
cmake_minimum_required (VERSION 3.24)

file(GLOB_RECURSE PROJECT_HEADERS "${SOLUTION_CMAKE_SOURCE_PATH}/include/Game/*.h")
file(GLOB_RECURSE PROJECT_CPPS *.cpp)

auto_header_group(Game "${PROJECT_HEADERS}" DST_FILES)
auto_source_group(Game "${PROJECT_CPPS}" DST_HEADERS)

add_library(Game SHARED ${PROJECT_HEADERS} ${PROJECT_CPPS})

add_compile_definitions(GAME_EXPORT)

add_dependencies(Game Core Graphics) 
target_link_libraries(Game Core Graphics)

# CMake 最低版本号要求
cmake_minimum_required (VERSION 2.8)
# 项目信息

file(GLOB_RECURSE PROJECT_HEADERS *.h)
file(GLOB_RECURSE PROJECT_CPPS *.cpp)

include_directories(
    ${SDK_PATH}DirectXTK12
    ${SDK_PATH}DirectXTK12/Src
    ${SDK_PATH}DirectXTK12/Inc
)

set(ALL_FILES ${PROJECT_HEADERS} ${PROJECT_CPPS})

auto_source_group("${ALL_FILES}" DST_FILES)

add_library(render STATIC ${DST_FILES})

add_dependencies(render core window)
target_link_libraries(render core window assimp)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /bigobj")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
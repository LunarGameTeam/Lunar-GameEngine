# CMake 最低版本号要求
cmake_minimum_required (VERSION 3.24)

# 项目信息
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

file(GLOB_RECURSE PROJECT_HEADERS "${SOLUTION_CMAKE_SOURCE_PATH}/include/Animation/*.h")
file(GLOB_RECURSE PROJECT_CPPS "Animation/*")

source_group(TREE "${SOLUTION_CMAKE_SOURCE_PATH}/include" PREFIX Public FILES ${PROJECT_HEADERS} )
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" PREFIX Private FILES ${PROJECT_CPPS})

add_library(Animation SHARED ${PROJECT_CPPS} ${PROJECT_HEADERS})

add_compile_definitions(ANIMATION_EXPORT)

add_dependencies(Animation Core)

target_link_libraries(Animation Core)

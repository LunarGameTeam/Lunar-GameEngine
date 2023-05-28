# CMake 最低版本号要求
cmake_minimum_required (VERSION 3.24)

file(GLOB_RECURSE PROJECT_HEADERS "${SOLUTION_CMAKE_SOURCE_PATH}/include/Game/*.h")
file(GLOB_RECURSE PROJECT_CPPS *.cpp)

source_group(TREE "${SOLUTION_CMAKE_SOURCE_PATH}/include" PREFIX Public FILES ${PROJECT_HEADERS} )
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" PREFIX Private FILES ${PROJECT_CPPS} )

add_library(Game SHARED ${PROJECT_HEADERS} ${PROJECT_CPPS})

add_compile_definitions(GAME_EXPORT)

target_link_libraries(Game Core Graphics Animation)

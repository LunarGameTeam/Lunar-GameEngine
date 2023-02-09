# CMake 最低版本号要求
cmake_minimum_required (VERSION 3.24)
# 项目信息

file(GLOB_RECURSE PROJECT_HEADERS *.h)
file(GLOB_RECURSE PROJECT_CPPS *.cpp)

set(ALL_FILES ${PROJECT_HEADERS} ${PROJECT_CPPS})

source_group(TREE "${SOLUTION_CMAKE_SOURCE_PATH}/include" PREFIX Public FILES ${PROJECT_HEADERS} )
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" PREFIX Private FILES ${PROJECT_CPPS} )

add_executable(CoreTest ${PROJECT_CPPS})

set_property(TARGET CoreTest PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${SOLUTION_CMAKE_SOURCE_PATH}")

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

add_dependencies(CoreTest Core GTest::gtest_main)
target_link_libraries(CoreTest Core GTest::gtest_main)


# CMake 最低版本号要求
cmake_minimum_required (VERSION 2.8)
# 项目信息

file(GLOB_RECURSE PROJECT_HEADERS *.h)
file(GLOB_RECURSE PROJECT_CPPS *.cpp)

set(ALL_FILES ${PROJECT_HEADERS} ${PROJECT_CPPS})

auto_source_group("${ALL_FILES}" DST_FILES)


add_executable(editor ${DST_FILES})
set_property(TARGET editor PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${SOLUTION_CMAKE_SOURCE_PATH}")
add_dependencies(editor core window)

target_link_libraries(editor core window)


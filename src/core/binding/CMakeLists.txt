# CMake 最低版本号要求
cmake_minimum_required (VERSION 3.24)
# 项目信息

file(GLOB_RECURSE PROJECT_HEADERS *.h)
file(GLOB_RECURSE PROJECT_CPPS *.cpp)
set(ALL_FILES ${PROJECT_HEADERS} ${PROJECT_CPPS})

add_compile_definitions(BINDING_EXPORT)

auto_source_group("${ALL_FILES}" DST_FILES)
add_library(binding SHARED ${DST_FILES})
add_dependencies(binding core)
target_link_libraries(binding ${PYTHON_LIBRARIES} core)
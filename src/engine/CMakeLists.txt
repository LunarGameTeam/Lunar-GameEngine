# CMake 最低版本号要求
cmake_minimum_required (VERSION 3.24)

file(GLOB_RECURSE PROJECT_HEADERS *.h)
file(GLOB_RECURSE PROJECT_CPPS *.cpp)

set(ALL_FILES ${PROJECT_HEADERS} ${PROJECT_CPPS})
auto_source_group("${ALL_FILES}" DST_FILES)

add_library(engine SHARED ${DST_FILES})

add_compile_definitions(ENGINE_EXPORT)

add_dependencies(engine core window render) 
target_link_libraries(engine core window render)

set_target_properties(engine PROPERTIES
    SUFFIX ".pyd"
    OUTPUT_NAME "luna_d"
)
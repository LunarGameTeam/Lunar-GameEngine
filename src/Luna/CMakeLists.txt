# CMake 最低版本号要求
cmake_minimum_required (VERSION 3.24)

file(GLOB_RECURSE PROJECT_HEADERS *.h)
file(GLOB_RECURSE PROJECT_CPPS *.cpp)

set(ALL_FILES ${PROJECT_HEADERS} ${PROJECT_CPPS})
auto_source_group(Luna "${ALL_FILES}" DST_FILES)

add_library(Luna SHARED ${DST_FILES})

add_compile_definitions(LUNA_EXPORT)

target_link_libraries(Luna Core)

if (USING_DEBUG_PYTHON)
    set_target_properties(Luna PROPERTIES
        SUFFIX ".pyd"
        OUTPUT_NAME "luna_d")
else()
    set_target_properties(Luna PROPERTIES
        SUFFIX ".pyd"
        OUTPUT_NAME "luna")
endif()
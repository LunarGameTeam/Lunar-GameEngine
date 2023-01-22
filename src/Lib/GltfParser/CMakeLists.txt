# CMake 最低版本号要求
cmake_minimum_required (VERSION 3.24)

# 项目信息
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

file(GLOB_RECURSE PROJECT_HEADERS "${SOLUTION_CMAKE_SOURCE_PATH}/include/Lib/GltfParser/*.h")
file(GLOB_RECURSE PROJECT_CPPS *.h *.cpp *.cc *.c)

auto_header_group(GltfParser "${PROJECT_HEADERS}" DST_FILES /Lib/GltfParser)
auto_source_group(GltfParser "${PROJECT_CPPS}" DST_HEADERS Lib/)

add_library(GltfParser SHARED ${DST_FILES} ${DST_HEADERS} ${NATVIS_PATH})

add_compile_definitions(GLTF_EXPORT)

add_dependencies(GltfParser ImportScene GLTFSDK)

target_link_libraries(GltfParser ImportScene GLTFSDK)

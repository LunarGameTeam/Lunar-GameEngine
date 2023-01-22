# CMake 最低版本号要求
cmake_minimum_required (VERSION 3.24)

# 项目信息
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

file(GLOB_RECURSE PROJECT_HEADERS "${SOLUTION_CMAKE_SOURCE_PATH}/include/Lib/ObjParser/*.h")
file(GLOB_RECURSE PROJECT_CPPS *.h *.cpp *.cc *.c)

auto_header_group(ObjParser "${PROJECT_HEADERS}" DST_FILES /Lib/ObjParser)
auto_source_group(ObjParser "${PROJECT_CPPS}" DST_HEADERS Lib/)

add_library(ObjParser SHARED ${DST_FILES} ${DST_HEADERS} ${NATVIS_PATH})

add_compile_definitions(OBJ_EXPORT)

add_dependencies(ObjParser ImportScene tinyobjloader::tinyobjloader)

target_link_libraries(ObjParser ImportScene tinyobjloader::tinyobjloader)

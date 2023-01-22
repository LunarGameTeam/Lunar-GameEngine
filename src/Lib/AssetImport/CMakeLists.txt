# CMake 最低版本号要求
cmake_minimum_required (VERSION 3.24)

# 项目信息
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

file(GLOB_RECURSE PROJECT_HEADERS "${SOLUTION_CMAKE_SOURCE_PATH}/include/Lib/AssetImport/*.h")
file(GLOB_RECURSE PROJECT_CPPS *.h *.cpp *.cc *.c)

auto_header_group(AssetImport "${PROJECT_HEADERS}" DST_FILES /Lib/AssetImport)
auto_source_group(AssetImport "${PROJECT_CPPS}" DST_HEADERS Lib/)

add_library(AssetImport SHARED ${DST_FILES} ${DST_HEADERS} ${NATVIS_PATH})

# add_compile_definitions(TRACY_EXPORTS)
# add_compile_definitions(IMGUI_EXPORT)
add_compile_definitions(ASSET_IMPORT_EXPORT)
# add_compile_definitions(FMT_EXPORT)

add_dependencies(AssetImport ImportScene Graphics)

target_link_libraries(AssetImport ImportScene Graphics)

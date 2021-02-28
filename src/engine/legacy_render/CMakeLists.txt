# CMake 最低版本号要求
cmake_minimum_required (VERSION 2.8)
# 项目信息

file(GLOB_RECURSE PROJECT_HEADERS *.h)
file(GLOB_RECURSE PROJECT_CPPS *.cpp)


set(ALL_FILES ${PROJECT_HEADERS} ${PROJECT_CPPS})

auto_source_group("${ALL_FILES}" DST_FILES)

add_compile_definitions(LEGACY_RENDER_EXPORT)

add_library(legacy_render SHARED ${DST_FILES})

add_dependencies(legacy_render core window)
target_link_libraries(legacy_render core d3d11 DirectXTK dxgi dxguid window D3dcompiler)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /bigobj")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
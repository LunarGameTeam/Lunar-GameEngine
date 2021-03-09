# CMake 最低版本号要求
cmake_minimum_required (VERSION 2.8)
# 项目信息

file(GLOB_RECURSE PROJECT_HEADERS *.h)
file(GLOB_RECURSE PROJECT_CPPS *.cpp)


set(ALL_FILES ${PROJECT_HEADERS} ${PROJECT_CPPS})

auto_source_group("${ALL_FILES}" DST_FILES)

add_compile_definitions(LEGACY_RENDER_EXPORT)

add_library(legacy_render SHARED ${DST_FILES})

add_dependencies(legacy_render core assimp)
target_link_libraries(legacy_render core d3d11 DirectXTK dxgi dxguid window D3dcompiler assimp)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /bigobj")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")

ADD_CUSTOM_COMMAND(TARGET legacy_render COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_BINARY_DIR}/sdk/assimp/bin/Debug/assimp-vc142-mtd.dll	${EXECUTABLE_OUTPUT_PATH}/Debug/assimp-vc142-mtd.dll VERBATIM)

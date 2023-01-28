# CMake 最低版本号要求
cmake_minimum_required (VERSION 3.24)

# 项目信息
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

file(GLOB_RECURSE PROJECT_HEADERS "${SOLUTION_CMAKE_SOURCE_PATH}/include/Lib/ResourceImportBinding/*.h")
file(GLOB_RECURSE PROJECT_CPPS *.h *.cpp *.cc *.c)

auto_header_group(ResourceImportBinding "${PROJECT_HEADERS}" DST_FILES /Lib/ResourceImportBinding)
auto_source_group(ResourceImportBinding "${PROJECT_CPPS}" DST_HEADERS Lib/)

add_library(ResourceImportBinding SHARED ${DST_FILES} ${DST_HEADERS} ${NATVIS_PATH})

add_compile_definitions(RESOURCE_EXPORT)

if(LUNA_USING_FBX_TOOLS)
	add_compile_definitions(FBX_LIB_INITED)
	add_dependencies(ResourceImportBinding FbxParser)
	target_link_libraries(ResourceImportBinding FbxParser)
endif()
if(LUNA_USING_OBJ_TOOLS)
	add_compile_definitions(OBJ_LIB_INITED)
	add_dependencies(ResourceImportBinding ObjParser)
	target_link_libraries(ResourceImportBinding ObjParser)
endif()
if(LUNA_USING_GLTF_TOOLS)
	add_compile_definitions(GLTF_LIB_INITED)
	add_dependencies(ResourceImportBinding GltfParser)
	target_link_libraries(ResourceImportBinding GltfParser)
endif()

add_dependencies(ResourceImportBinding AssetImport)

target_link_libraries(ResourceImportBinding AssetImport)

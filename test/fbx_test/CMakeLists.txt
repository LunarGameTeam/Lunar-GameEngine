# CMake 最低版本号要求
cmake_minimum_required (VERSION 3.24)
# 项目信息

file(GLOB_RECURSE PROJECT_HEADERS *.h)
file(GLOB_RECURSE PROJECT_CPPS *.cpp)


set(ALL_FILES ${PROJECT_HEADERS} ${PROJECT_CPPS})

auto_source_group(fbx_test "${ALL_FILES}" DST_FILES "")

add_executable(fbx_test ${DST_FILES})


add_compile_definitions(FBX_LIB_INITED)
add_dependencies(fbx_test FbxParser)
target_link_libraries(fbx_test FbxParser)


add_compile_definitions(OBJ_LIB_INITED)
add_dependencies(fbx_test ObjParser)
target_link_libraries(fbx_test ObjParser)

add_compile_definitions(GLTF_LIB_INITED)
add_dependencies(fbx_test GltfParser)
target_link_libraries(fbx_test GltfParser)

add_dependencies(fbx_test AssetImport)
target_link_libraries(fbx_test AssetImport)


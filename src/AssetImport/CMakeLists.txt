# CMake 最低版本号要求
cmake_minimum_required (VERSION 3.24)

# 项目信息
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

file(GLOB_RECURSE PROJECT_HEADERS "${SOLUTION_CMAKE_SOURCE_PATH}/include/AssetImport/*.h")
file(GLOB_RECURSE PROJECT_CPPS "AssetImport/*")
list(FILTER PROJECT_CPPS EXCLUDE REGEX "AssetImport/FbxParser/*")


auto_header_group(AssetImport "${PROJECT_HEADERS}" PROJECT_HEADERS "")
auto_source_group(AssetImport "${PROJECT_CPPS}" PROJECT_CPPS "")
add_library(AssetImport SHARED ${PROJECT_CPPS} ${PROJECT_HEADERS})

file(GLOB_RECURSE FBX_CPPS "AssetImport/FbxParser/*")
auto_source_group(AssetImport "${FBX_CPPS}" FBX_CPPS "")
target_sources(AssetImport PRIVATE
                ${FBX_CPPS})

add_compile_definitions(ASSET_IMPORT_EXPORT)

add_compile_definitions(FBXSDK_SHARED)
add_compile_definitions(FBX_EXPORT)

set(FBX_ROOT_DIR ${SOLUTION_CMAKE_SOURCE_PATH}/third-party/fbx-sdk-2020.2.1)

INCLUDE_DIRECTORIES(BEFORE "${FBX_ROOT_DIR}/include")
IF(WIN32)
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(FBX_LIB_DIR ${FBX_ROOT_DIR}/lib/libfbxsdk.lib)
    elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
        set(FBX_LIB_DIR ${FBX_ROOT_DIR}/lib/libfbxsdk.lib)
    endif()
ENDIF()
target_link_libraries(AssetImport ${FBX_LIB_DIR})    
add_custom_command(TARGET AssetImport
    POST_BUILD  
    COMMAND xcopy "\"${FBX_ROOT_DIR}/lib\\*.dll\"" "\"$(OutDir)\"" /Y
)

add_dependencies(AssetImport Core Graphics GLTFSDK tinyobjloader::tinyobjloader)

target_link_libraries(AssetImport Core Graphics GLTFSDK tinyobjloader::tinyobjloader)

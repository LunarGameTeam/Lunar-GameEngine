# CMake 最低版本号要求
cmake_minimum_required (VERSION 3.24)
# 项目信息

SET( Sources
    fbx_importer_func.h
    fbx_importer_func.cpp
    )
	
SET( FbxImporter
	fbx_importer/fbx_scene_importer.h
    fbx_importer/fbx_scene_importer.cpp
	fbx_importer/fbx_mesh_importer.h
    fbx_importer/fbx_mesh_importer.cpp
	fbx_importer/fbx_skeleton_importer.h
    fbx_importer/fbx_skeleton_importer.cpp
	fbx_importer/fbx_animation_importer.h
    fbx_importer/fbx_animation_importer.cpp
	fbx_importer/fbx_camera_importer.h
    fbx_importer/fbx_camera_importer.cpp
	fbx_importer/fbx_material_importer.h
    fbx_importer/fbx_material_importer.cpp
	fbx_importer/fbx_importer_common.h
    fbx_importer/fbx_importer_common.cpp
	fbx_importer/fbx_importer_util.h
    fbx_importer/fbx_importer_util.cpp
    )

SET( FbxLoader
	fbx_loader/fbx_scene_loader.h
	fbx_loader/fbx_scene_loader.cpp
	fbx_loader/fbx_mesh_loader.h
	fbx_loader/fbx_mesh_loader.cpp
	fbx_loader/fbx_skeleton_loader.h
	fbx_loader/fbx_skeleton_loader.cpp
	fbx_loader/fbx_animation_loader.h
	fbx_loader/fbx_animation_loader.cpp
	fbx_loader/fbx_material_loader.h
	fbx_loader/fbx_material_loader.cpp
	fbx_loader/fbx_camera_loader.h
	fbx_loader/fbx_camera_loader.cpp
	fbx_loader/fbx_loader_common.h
	fbx_loader/fbx_loader_common.cpp
	fbx_loader/fbx_loader_util.h
	fbx_loader/fbx_loader_util.cpp
    )
source_group(source  FILES ${Sources})
source_group(source/fbx_importer  FILES ${FbxImporter})
source_group(source/fbx_loader  FILES ${FbxLoader})

set(ALL_FILES ${PROJECT_HEADERS} ${PROJECT_CPPS})

auto_source_group(fbx_helper "${ALL_FILES}" DST_FILES)

add_library(fbx_helper SHARED ${Sources} ${FbxImporter} ${FbxLoader})
add_compile_definitions(FBXSDK_SHARED)

find_path(FBX_ROOT_DIR DOC "Path to FBX SDK folder" REQUIRED)

INCLUDE_DIRECTORIES(BEFORE "${FBX_ROOT_DIR}/include")
INCLUDE_DIRECTORIES(BEFORE "${LIBRARY_OUTPUT_PATH}/111")
IF(WIN32)
	if(CMAKE_SIZEOF_VOID_P EQUAL 8)
		set(FBX_LIB_DIR ${FBX_ROOT_DIR}/lib/vs2019/x64/release/libfbxsdk.lib)
	elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
		set(FBX_LIB_DIR ${FBX_ROOT_DIR}/lib/vs2019/x86/release/libfbxsdk.lib)
	endif()
ENDIF()
add_dependencies(fbx_helper core)

target_link_libraries(fbx_helper core ${FBX_LIB_DIR})

add_custom_command(TARGET fbx_helper
  POST_BUILD  
  COMMAND xcopy ${FBX_ROOT_DIR}/lib/vs2019/x64/release\\*.dll "\"$(OutDir)\"" /Y
)
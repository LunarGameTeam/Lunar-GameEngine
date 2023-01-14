# CMake 最低版本号要求
cmake_minimum_required (VERSION 3.24)
# 项目信息

SET( Sources
    gltf_importer_func.h
    gltf_importer_func.cpp
    )
	
SET( FbxImporter
	gltf_importer/gltf_scene_importer.h
    gltf_importer/gltf_scene_importer.cpp
	gltf_importer/gltf_mesh_importer.h
    gltf_importer/gltf_mesh_importer.cpp
	gltf_importer/gltf_skeleton_importer.h
    gltf_importer/gltf_skeleton_importer.cpp
	gltf_importer/gltf_animation_importer.h
    gltf_importer/gltf_animation_importer.cpp
	gltf_importer/gltf_camera_importer.h
    gltf_importer/gltf_camera_importer.cpp
	gltf_importer/gltf_material_importer.h
    gltf_importer/gltf_material_importer.cpp
	gltf_importer/gltf_importer_common.h
    gltf_importer/gltf_importer_common.cpp
	gltf_importer/gltf_importer_util.h
    gltf_importer/gltf_importer_util.cpp
    )

SET( FbxLoader
	gltf_loader/gltf_scene_loader.h
	gltf_loader/gltf_scene_loader.cpp
	gltf_loader/gltf_mesh_loader.h
	gltf_loader/gltf_mesh_loader.cpp
	gltf_loader/gltf_skeleton_loader.h
	gltf_loader/gltf_skeleton_loader.cpp
	gltf_loader/gltf_animation_loader.h
	gltf_loader/gltf_animation_loader.cpp
	gltf_loader/gltf_material_loader.h
	gltf_loader/gltf_material_loader.cpp
	gltf_loader/gltf_camera_loader.h
	gltf_loader/gltf_camera_loader.cpp
	gltf_loader/gltf_loader_common.h
	gltf_loader/gltf_loader_common.cpp
	gltf_loader/gltf_loader_util.h
	gltf_loader/gltf_loader_util.cpp
    )
source_group(source  FILES ${Sources})
source_group(source/gltf_importer  FILES ${FbxImporter})
source_group(source/gltf_loader  FILES ${FbxLoader})

add_library(gltf_helper SHARED ${Sources} ${FbxImporter} ${FbxLoader})

add_dependencies(gltf_helper import_scene core GLTFSDK)

target_link_libraries(gltf_helper import_scene core GLTFSDK)
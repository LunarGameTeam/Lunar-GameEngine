# CMake 最低版本号要求
cmake_minimum_required (VERSION 3.24)
# 项目信息

SET( Sources
    obj_importer_func.h
    obj_importer_func.cpp
    )
	
SET( objImporter
	obj_importer/obj_scene_importer.h
    obj_importer/obj_scene_importer.cpp
	obj_importer/obj_mesh_importer.h
    obj_importer/obj_mesh_importer.cpp
	obj_importer/obj_material_importer.h
    obj_importer/obj_material_importer.cpp
	obj_importer/obj_importer_common.h
    obj_importer/obj_importer_common.cpp
	obj_importer/obj_importer_util.h
    obj_importer/obj_importer_util.cpp
    )

SET( objLoader
	obj_loader/obj_scene_loader.h
	obj_loader/obj_scene_loader.cpp
	obj_loader/obj_mesh_loader.h
	obj_loader/obj_mesh_loader.cpp
	obj_loader/obj_material_loader.h
	obj_loader/obj_material_loader.cpp
	obj_loader/obj_loader_common.h
	obj_loader/obj_loader_common.cpp
	obj_loader/obj_loader_util.h
	obj_loader/obj_loader_util.cpp
    )
source_group(source  FILES ${Sources})
source_group(source/obj_importer  FILES ${objImporter})
source_group(source/obj_loader  FILES ${objLoader})

add_library(obj_helper SHARED ${Sources} ${objImporter} ${objLoader})

add_dependencies(obj_helper import_scene core tinyobjloader::tinyobjloader)

target_link_libraries(obj_helper import_scene core tinyobjloader::tinyobjloader)
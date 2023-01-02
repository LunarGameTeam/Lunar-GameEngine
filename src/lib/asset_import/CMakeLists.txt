# CMake 最低版本号要求
cmake_minimum_required (VERSION 3.24)
# 项目信息
SET( Sources
	asset_import_common.h
	asset_import_common.cpp
	mesh_asset_import.h
    mesh_asset_import.cpp
    )
source_group(Source  FILES ${Sources})

add_library(asset_import SHARED ${Sources})

add_dependencies(import_scene asset_import)

target_link_libraries(import_scene asset_import)

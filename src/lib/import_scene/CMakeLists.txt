# CMake 最低版本号要求
cmake_minimum_required (VERSION 3.24)
# 项目信息
SET( Sources
	import_data_common.h
	import_data_common.cpp
    import_scene.h
    import_scene.cpp
	forsythtriangleorderoptimizer.h
	forsythtriangleorderoptimizer.cpp
	mikktspace.h
	mikktspace.cpp
	import_data_mesh.h
	import_data_mesh.cpp
    )
source_group(Source  FILES ${Sources})

add_library(import_scene SHARED ${Sources})

add_dependencies(import_scene core)

target_link_libraries(import_scene core)

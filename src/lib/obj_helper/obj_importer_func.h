#pragma once
#include "core/core_library.h"
#include "../import_scene/import_scene.h"
#ifndef OBJ_API
#ifdef OBJ_EXPORT
#define OBJ_API __declspec( dllexport )//宏定义
#else
#define OBJ_API __declspec( dllimport )
#endif
#endif
namespace luna::lobj
{
	void OBJ_API ImportFbxToLunaMesh(const LString& fbx_file_path, resimport::LImportScene& importScene);
}
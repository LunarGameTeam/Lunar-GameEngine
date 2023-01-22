#pragma once
#include "Core/CoreMin.h"
#include "Lib/ImportScene/ImportScene.h"
#ifndef OBJ_API
#ifdef OBJ_EXPORT
#define OBJ_API __declspec( dllexport )//宏定义
#else
#define OBJ_API __declspec( dllimport )
#endif
#endif
namespace luna::lobj
{
	void OBJ_API ImportObjToLunaResource(const LString& obj_file_path, resimport::LImportScene& importScene);
}
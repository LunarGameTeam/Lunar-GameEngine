#pragma once
#include "Core/CoreMin.h"
#include "lib/ImportScene/ImportScene.h"
#ifndef FBX_API
#ifdef FBX_EXPORT
#define FBX_API __declspec( dllexport )//宏定义
#else
#define FBX_API __declspec( dllimport )
#endif
#endif
namespace luna::lfbx
{
	void FBX_API ImportFbxToLunaResource(const LString& fbx_file_path, resimport::LImportScene& importScene);
}
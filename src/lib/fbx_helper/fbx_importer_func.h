#pragma once
#include "Core/core_library.h"
#include "Graphics/Asset/mesh_asset.h"
#ifndef FBX_API
#ifdef FBX_EXPORT
#define FBX_API __declspec( dllexport )//宏定义
#else
#define FBX_API __declspec( dllimport )
#endif
#endif
namespace luna::lfbx
{
	void FBX_API ImportFbxToLunaMesh(const LString& fbx_file_path, ImportData::LImportScene& importScene);
}
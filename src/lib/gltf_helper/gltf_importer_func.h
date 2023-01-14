#pragma once
#include "core/core_library.h"
#include "../import_scene/import_scene.h"
#ifndef GLTF_API
#ifdef GLTF_EXPORT
#define GLTF_API __declspec( dllexport )//宏定义
#else
#define GLTF_API __declspec( dllimport )
#endif
#endif
namespace luna::lgltf
{
	void GLTF_API ImportGltfToLunaMesh(const LString& gltf_file_path, resimport::LImportScene& importScene);
}
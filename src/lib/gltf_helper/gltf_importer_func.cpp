#pragma once
#define GLTF_EXPORT
#include "gltf_loader/gltf_scene_loader.h"
#include "gltf_importer/gltf_scene_importer.h"
#include "gltf_importer_func.h"
namespace luna::lgltf
{
	void ImportGltfToLunaResource(const LString& gltf_file_path, resimport::LImportScene &importScene)
	{
		LGltfLoaderHelper importer;
		LGltfSceneData gltfScene;
		importer.LoadGltfFile(gltf_file_path, gltfScene);
		instanceGltfSceneImport->ParseScene(&gltfScene, importScene);
	}
}
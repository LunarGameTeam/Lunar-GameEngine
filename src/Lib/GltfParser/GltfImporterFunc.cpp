#pragma once
#include "GltfLoader/GltfSceneLoader.h"
#include "GltfImporter/GltfSceneImporter.h"
#include "Lib/GltfParser/GltfImporterFunc.h"
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
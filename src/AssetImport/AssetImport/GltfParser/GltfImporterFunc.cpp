#pragma once
#include "GltfLoader/GltfSceneLoader.h"
#include "GltfImporter/GltfSceneImporter.h"

#include "AssetImport/GltfParser/GltfImporterFunc.h"

using namespace luna::asset;

namespace luna::asset
{

static std::shared_ptr<LGltfSceneImportHelper> instanceGltfSceneImport = std::make_shared<LGltfSceneImportHelper>();

void ImportGltfToLunaResource(const LString& gltf_file_path, asset::LImportScene& importScene)
{
	LGltfLoaderHelper importer;
	LGltfSceneData gltfScene;
	importer.LoadGltfFile(gltf_file_path, gltfScene);
	instanceGltfSceneImport->ParseScene(&gltfScene, importScene);
}

}
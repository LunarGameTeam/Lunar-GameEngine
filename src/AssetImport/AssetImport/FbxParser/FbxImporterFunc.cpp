#pragma once
#include "AssetImport/FbxParser/FbxLoader/FbxSceneLoader.h"
#include "AssetImport/FbxParser/FbxImporter/FbxSceneImporter.h"
#include "AssetImport/FbxParser/FbxImporterFunc.h"

using namespace luna::lfbx;

namespace luna::asset
{

void ImportFbxToLunaResource(const LString& fbx_file_path, asset::LImportScene& importScene)
{
	LFbxLoaderHelper importer;
	LFbxSceneData fbxScene;
	importer.LoadFbxFile(fbx_file_path, fbxScene);
	instanceFbxSceneImport->ParseScene(&fbxScene, importScene);

}
}
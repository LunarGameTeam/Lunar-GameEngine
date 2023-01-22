#pragma once
#include "FbxLoader/FbxSceneLoader.h"
#include "FbxImporter/FbxSceneImporter.h"
#include "Lib/FbxParser/FbxImporterFunc.h"
namespace luna::lfbx
{
	void ImportFbxToLunaResource(const LString& fbx_file_path, resimport::LImportScene &importScene)
	{
		LFbxLoaderHelper importer;
		LFbxSceneData fbxScene;
		importer.LoadFbxFile(fbx_file_path, fbxScene);
		instanceFbxSceneImport->ParseScene(&fbxScene, importScene);
	}
}
#pragma once
#define FBX_EXPORT
#include "fbx_loader/fbx_scene_loader.h"
#include "fbx_importer/fbx_scene_importer.h"
#include "fbx_importer_func.h"
namespace luna::lfbx
{
	void ImportFbxToLunaMesh(const LString& fbx_file_path, ImportData::LImportScene &importScene)
	{
		LFbxLoaderHelper importer;
		LFbxSceneData fbxScene;
		importer.LoadFbxFile(fbx_file_path, fbxScene);
		instanceFbxSceneImport->ParseScene(&fbxScene, importScene);
	}
}
#pragma once

#include "AssetImport/ObjParser/ObjImporterFunc.h"

#include "ObjLoader/ObjSceneLoader.h"
#include "ObjImporter/ObjSceneImporter.h"

using namespace luna::asset;

namespace luna::asset
{
	void ImportObjToLunaResource(const LString& obj_file_path, asset::LImportScene &importScene)
	{
		LObjLoaderHelper importer;
		LObjSceneData objScene;
		importer.LoadObjFile(obj_file_path, objScene);
		instanceObjSceneImport->ParseScene(&objScene, importScene);
	}
}
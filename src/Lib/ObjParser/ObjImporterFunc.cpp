#pragma once
#include "ObjLoader/ObjSceneLoader.h"
#include "ObjImporter/ObjSceneImporter.h"
#include "Lib/ObjParser/ObjImporterFunc.h"
namespace luna::lobj
{
	void ImportObjToLunaResource(const LString& obj_file_path, resimport::LImportScene &importScene)
	{
		LObjLoaderHelper importer;
		LObjSceneData objScene;
		importer.LoadObjFile(obj_file_path, objScene);
		instanceObjSceneImport->ParseScene(&objScene, importScene);
	}
}
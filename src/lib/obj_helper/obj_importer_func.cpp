#pragma once
#define OBJ_EXPORT
#include "obj_loader/obj_scene_loader.h"
#include "obj_importer/obj_scene_importer.h"
#include "obj_importer_func.h"
namespace luna::lobj
{
	void ImportObjToLunaMesh(const LString& obj_file_path, resimport::LImportScene &importScene)
	{
		LObjLoaderHelper importer;
		LObjSceneData objScene;
		importer.LoadObjFile(obj_file_path, objScene);
		//instanceObjSceneImport->ParseScene(&objScene, importScene);
	}
}
#pragma once
#include "Core/CoreMin.h"
#include "AssetImport/ImportScene/ImportScene.h"

namespace luna::asset
{
	void ASSET_IMPORT_API ImportObjToLunaResource(const LString& obj_file_path, asset::LImportScene& importScene);
}
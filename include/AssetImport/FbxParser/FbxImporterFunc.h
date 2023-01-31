#pragma once
#include "Core/CoreMin.h"
#include "AssetImport/ImportScene/ImportScene.h"

namespace luna::asset
{
	void ASSET_IMPORT_API ImportFbxToLunaResource(const LString& fbx_file_path, asset::LImportScene& importScene);
}
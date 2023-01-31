#pragma once
#include "Core/CoreMin.h"
#include "AssetImport/ImportScene/ImportScene.h"

namespace luna::asset
{

ASSET_IMPORT_API void ImportGltfToLunaResource(const LString& gltf_file_path, LImportScene& importScene);

}
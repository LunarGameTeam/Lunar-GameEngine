#pragma once
#include "Core/CoreMin.h"
#include "AssetImport/ImportScene/ImportScene.h"
#include "Core/Scripting/Binding.h"
#include"Assetimport/SceneAssetImport.h"
namespace luna
{
	void ASSET_IMPORT_API ImportFileToLunaResource(
		const LString& filePath,
		const LString& resSavePath,
		const LString& fileName,
		const LString& fileExtension
	);

	void ASSET_IMPORT_API ImportFileToLunaSceneResource(
		const LString& filePath,
		const LString& resSavePath,
		const LString& fileName,
		const LString& fileExtension,
		asset::LSceneAssetNodeMessage* nodeMessage
	);
}
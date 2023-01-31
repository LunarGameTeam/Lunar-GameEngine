#pragma once
#include "Core/CoreMin.h"
#include "AssetImport/ImportScene/ImportScene.h"

namespace luna
{
	void ASSET_IMPORT_API ImportFileToLunaResource(
		const LString& filePath,
		const LString& resSavePath,
		const LString& fileName,
		const LString& fileExtension
	);
}
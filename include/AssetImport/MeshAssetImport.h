#pragma once
#include "AssetImport/AssetImportCommon.h"

namespace luna::asset
{

class ASSET_IMPORT_API LMeshAssetImport :public LAssetImportBase
{
private:
	void ParsingImportSceneImpl(const asset::LImportScene& importSceneData, LAssetPack& outAssetPack) override;
};

}


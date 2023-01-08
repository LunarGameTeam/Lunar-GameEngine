#pragma once
#include"asset_import_common.h"
namespace luna::assetimport
{
	class ASSET_IMPORT_API LMeshAssetImport :public LAssetImportBase
	{
	private:
		void ParsingImportSceneImpl(const resimport::LImportScene& importSceneData, LAssetPack& outAssetPack) override;
	};
}


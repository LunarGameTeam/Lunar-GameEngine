#pragma once

#include "AssetImport/ImportScene/ImportDataCommon.h"

namespace luna::asset
{
	class ASSET_IMPORT_API LImportNodeDataSkeletonAnimation : public LImportNodeDataBase
	{
	public:
		LImportNodeDataSkeletonAnimation(const size_t index);
	private:
		void ConvertDataAxisAndUnitImpl(bool hasReflectTransform, LMatrix4f convertInvMatrix, LMatrix4f convertMatrix) override;
	};
}


#pragma once

#include "core/math/math.h"
#include "core/asset/asset.h"
#include "core/foundation/string.h"
#include "render/render_config.h"

#include "mesh_asset.h"

namespace luna::render
{

class RENDER_API ObjAsset : public LTextAsset
{
	RegisterTypeEmbedd(ObjAsset, LTextAsset)
public:
	ObjAsset()
	{

	}

	SubMesh* GetSubMeshAt(size_t index)
	{
		return mSubMesh[index];
	};

	void OnAssetFileRead(LSharedPtr<Dictionary> meta, LSharedPtr<LFile> file) override;
private:
	LArray<SubMesh*> mSubMesh;
};
}

#pragma once

#include "Core/Math/Math.h"
#include "Core/Asset/Asset.h"
#include "Core/Foundation/String.h"
#include "Graphics/RenderConfig.h"

#include "MeshAsset.h"

namespace luna::render
{

class RENDER_API ObjAsset : public TextAsset
{
	RegisterTypeEmbedd(ObjAsset, TextAsset)
public:
	ObjAsset()
	{

	}

	SubMesh* GetSubMeshAt(size_t index)
	{
		return mSubMesh[index];
	};

	void OnAssetFileRead(LSharedPtr<JsonDict> meta, LSharedPtr<LFile> file) override;
private:
	LArray<SubMesh*> mSubMesh;
};
}

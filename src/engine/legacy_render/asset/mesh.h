#pragma once
#include "core/misc/container.h"
#include "core/asset/asset.h"

#include "legacy_render/private_render.h"
#include "legacy_render/interface/i_model.h"

#include <d3d11.h>

namespace luna
{
namespace legacy_render
{

struct BaseVertex {
	LVector3f pos;
	LVector2f uv;
	LVector3f normal;
};

struct LEGACY_RENDER_API SubMesh
{
	LVector<BaseVertex> vertices;
	LVector<UINT> indices;
};

struct VertexInputLayout 
{
public:

};

class LEGACY_RENDER_API Mesh : public LBasicAsset, public IMesh
{
public:
	void OnAssetFileLoad(LSharedPtr<AssetMetaData> meta, LSharedPtr<LFile> file) override;
private:
	LVector<SubMesh> m_sub_meshes;
	int m_vertex_count, m_index_count;
};

}
}

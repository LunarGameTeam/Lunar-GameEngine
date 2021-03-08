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

struct VERTEX {
	LVector3f pos;
	LVector2f uv;
};

struct LEGACY_RENDER_API SubMesh
{
	LVector<VERTEX> vertices;
	LVector<UINT> indices;
	ID3D11Buffer *vertex_buffer, *index_buffer;
	void Bind();
	void Draw();
};

class LEGACY_RENDER_API Mesh : public LBasicAsset, public IMesh
{
public:
	void OnAssetFileLoad(LSharedPtr<AssetMetaData> meta, LSharedPtr<LFile> file) override;

public:
	bool Init();
	void Bind();
	void Draw();
	const LMatrix4f& GetWolrdMatrix();

private:
	LVector3f m_pos = LVector3f(0,0,10);
	LQuaternion m_rotation = LQuaternion::Identity();
	LVector3f m_scale = LVector3f(0.01f,0.01f,0.01f);

	LVector<SubMesh> m_sub_meshes;

	ID3D11Buffer *m_vertex_buffer, *m_index_buffer;
	int m_vertex_count, m_index_count;
	LMatrix4f m_cache_matrix;


};

}
}

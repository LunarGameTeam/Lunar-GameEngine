#pragma once

#include "legacy_render/private_render.h"
#include "legacy_render/asset/material.h"
#include "legacy_render/asset/mesh.h"
#include "legacy_render/asset/texture.h"
#include "legacy_render/d3d11/shader.h"
#include "core/math/math.h"


namespace luna
{
namespace legacy_render
{


struct LEGACY_RENDER_API DX11MeshResource
{
	ID3D11Buffer *vertex_buffer;
	ID3D11Buffer *index_buffer;
	size_t num;
	void Update(Mesh *mesh);
};

struct LEGACY_RENDER_API DX11TextureResource
{
	ID3D11Texture2D *texture_buffer;
	ID3D11ShaderResourceView *resource_view;
	void Update(Texture *texture);
};

struct LEGACY_RENDER_API RenderNode
{
	LSharedPtr<Mesh> mesh;
	LSharedPtr<Material> material;
};

struct LEGACY_RENDER_API DX11RenderNode
{
	LSharedPtr<DX11MeshResource> mesh = MakeShared<DX11MeshResource>();
	LSharedPtr<DX11TextureResource> texture = MakeShared<DX11TextureResource>();
	LSharedPtr<Dx11Shader> shader;
	LMatrix4f world_matrix;
};

}
}
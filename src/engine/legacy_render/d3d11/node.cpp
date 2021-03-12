#include "node.h"
#include "legacy_render/render_subsystem.h"

namespace luna
{
namespace legacy_render
{

void DX11MeshResource::Update(Mesh *mesh)
{
	static auto *device = g_render_sys->GetDevice()->GetD3DDevice();
	static auto *context = g_render_sys->GetDevice()->GetD3DDeviceContext();
	for (SubMesh &submesh : mesh->GetSubMeshList())
	{
		D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
		D3D11_SUBRESOURCE_DATA vertexData, indexData;
		HRESULT result;
		// Set up the description of the static vertex buffer.
		vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		vertexBufferDesc.ByteWidth = sizeof(BaseVertex) * (UINT)submesh.vertices.size();
		vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vertexBufferDesc.CPUAccessFlags = 0;
		vertexBufferDesc.MiscFlags = 0;
		vertexBufferDesc.StructureByteStride = 0;

		// Give the subresource structure a pointer to the vertex data.
		vertexData.pSysMem = submesh.vertices.data();
		vertexData.SysMemPitch = 0;
		vertexData.SysMemSlicePitch = 0;

		// Now create the vertex buffer.
		result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &vertex_buffer);
		if (FAILED(result))
		{
		}

		// Set up the description of the static index buffer.
		indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		indexBufferDesc.ByteWidth = sizeof(unsigned long) * (UINT)submesh.indices.size();
		indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		indexBufferDesc.CPUAccessFlags = 0;
		indexBufferDesc.MiscFlags = 0;
		indexBufferDesc.StructureByteStride = 0;

		// Give the subresource structure a pointer to the index data.
		indexData.pSysMem = submesh.indices.data();
		indexData.SysMemPitch = 0;
		indexData.SysMemSlicePitch = 0;

		// Create the index buffer.
		result = device->CreateBuffer(&indexBufferDesc, &indexData, &index_buffer);
		if (FAILED(result))
		{
			return;
		}
		num = submesh.indices.size();
	}
}


void DX11TextureResource::Update(Texture *texture)
{
	static auto *device = g_render_sys->GetDevice()->GetD3DDevice();
	static auto *context = g_render_sys->GetDevice()->GetD3DDeviceContext();
	D3D11_TEXTURE2D_DESC desc;
	desc.Width = texture->GetWidth();
	desc.Height = texture->GetHeight();

	desc.MipLevels = desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UINT;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = texture->GetData();
	InitData.SysMemPitch = texture->GetWidth() * 4;
	InitData.SysMemSlicePitch = 0;

	HRESULT hr = device->CreateTexture2D(&desc, &InitData, &texture_buffer);

	if (FAILED(hr))
		return;

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UINT;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Texture2D.MostDetailedMip = 0;
	hr = device->CreateShaderResourceView(texture_buffer, &srvDesc, &resource_view);

	if (FAILED(hr))
		return;
}

}
}
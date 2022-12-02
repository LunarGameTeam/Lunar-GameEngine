#include "mesh_asset.h"
#include "render/render_module.h"
#include "core/core_library.h"
#include "core/framework/module.h"

#include <d3dcompiler.h>
#include <D3DCompiler.inl>

#include <algorithm>


namespace luna::render
{
RegisterTypeEmbedd_Imp(SubMesh)
{
	cls->Ctor<SubMesh>();
	cls->Property<&Self::mVertexCount>("vertex_size");
	cls->Property<&Self::mIndexCount>("index_size");

	cls->Binding<Self>();
	LBindingModule::Get("luna")->AddType(cls);
};

RegisterTypeEmbedd_Imp(MeshAsset)
{
	cls->Binding<Self>();
	LBindingModule::Get("luna")->AddType(cls);
	cls->Ctor<MeshAsset>();
	cls->Property<&Self::mSubMesh>("submesh");
};

void SubMesh::Release()
{
	m_ready = false;
}

void SubMesh::Update()
{
	if (m_ready)
		return;
	Release();
	RHIBufferDesc desc;
	desc.mSize = sizeof(BaseVertex) * mVertexData.size();
	desc.mBufferUsage = RHIBufferUsage::VertexBufferBit;

	mVB = sRenderModule->mRenderDevice->CreateBuffer(desc, mVertexData.data());

	desc.mSize = sizeof(uint32_t) * mIndexData.size();
	desc.mBufferUsage = RHIBufferUsage::IndexBufferBit;
	mIB = sRenderModule->mRenderDevice->CreateBuffer( desc, mIndexData.data());
	m_ready = true;
}

void SubMesh::Init()
{
	if (m_ready)
		return;

	mVeretexLayout.AddVertexElement(VertexElementType::Float, VertexElementUsage::UsagePosition, 3);
	mVeretexLayout.AddVertexElement(VertexElementType::Float, VertexElementUsage::UsagePosition, 4);
	mVeretexLayout.AddVertexElement(VertexElementType::Float, VertexElementUsage::UsagePosition, 3);
	mVeretexLayout.AddVertexElement(VertexElementType::Float, VertexElementUsage::UsagePosition, 4);

	mVeretexLayout.AddVertexElement(VertexElementType::Float, VertexElementUsage::UsagePosition, 2);
	mVeretexLayout.AddVertexElement(VertexElementType::Float, VertexElementUsage::UsagePosition, 2);
	mVeretexLayout.AddVertexElement(VertexElementType::Float, VertexElementUsage::UsagePosition, 2);
	mVeretexLayout.AddVertexElement(VertexElementType::Float, VertexElementUsage::UsagePosition, 2);

	Update();
}

void MeshAsset::OnAssetBinaryRead(const byte* value)
{
	LType* obj_type = this->GetClass();
	auto prop = obj_type->GetProperty("submesh");
	TSubPtrArray<SubMesh>& ary = prop->GetValue<TSubPtrArray<SubMesh>>(this);
	const byte* ptr = value;
	for (TSubPtr<SubMesh>& it : mSubMesh)
	{
		it->mVertexData.resize(it->mVertexCount);
		it->mIndexData.resize(it->mIndexCount);
		memcpy(it->mVertexData.data(), ptr, it->mVertexCount * sizeof(BaseVertex));
		ptr += it->mVertexCount * sizeof(BaseVertex);
		memcpy(it->mIndexData.data(), ptr, it->mIndexCount * sizeof(uint32_t));
		ptr += it->mIndexCount * sizeof(uint32_t);
	}
	return;
}

void MeshAsset::OnAssetBinaryWrite(LVector<byte>& data)
{
	size_t globel_size = data.size();
	size_t offset = data.size();
	for (int32_t id = 0; id < mSubMesh.Size(); ++id)
	{
		globel_size += mSubMesh[id]->mVertexData.size() * sizeof(BaseVertex);
		globel_size += mSubMesh[id]->mIndexData.size() * sizeof(uint32_t);
	}
	data.resize(globel_size);
	byte* dst = data.data() + offset;
	for (int32_t idx = 0; idx < mSubMesh.Size(); ++idx)
	{
		memcpy(dst, mSubMesh[idx]->mVertexData.data(), mSubMesh[idx]->mVertexData.size() * sizeof(BaseVertex));
		dst += mSubMesh[idx]->mVertexData.size() * sizeof(BaseVertex);
		memcpy(dst, mSubMesh[idx]->mIndexData.data(), mSubMesh[idx]->mIndexData.size() * sizeof(uint32_t));
		dst += mSubMesh[idx]->mIndexData.size() * sizeof(uint32_t);
	}
};

// void SubMesh::Bind()
// {
// 	static RenderSubusystem *render = gEngine->GetSubsystem<RenderSubusystem>();
// 	static auto *device = render->GetDevice()->GetD3DDevice();
// 	static auto *context = render->GetDevice()->GetD3DDeviceContext();
// 	unsigned int stride;
// 	unsigned int offset;
//
//
// 	// Set vertex buffer stride and offset.
// 	stride = sizeof(BaseVertex);
// 	offset = 0;
//
// 	// Set the vertex buffer to active in the input assembler so it can be rendered.
// 	context->IASetVertexBuffers(0, 1, &vertex_buffer, &stride, &offset);
//
// 	// Set the index buffer to active in the input assembler so it can be rendered.
// 	context->IASetIndexBuffer(index_buffer, DXGI_FORMAT_R32_UINT, 0);
//
// 	// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
// 	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
//  }

// void SubMesh::Draw()
// {
// 	static RenderSubusystem *render = gEngine->GetSubsystem<RenderSubusystem>();
// 	static auto *device = render->GetDevice()->GetD3DDevice();
// 	static auto *context = render->GetDevice()->GetD3DDeviceContext();
// 	context->DrawIndexed(indices.size(), 0, (UINT)0);
// }
}

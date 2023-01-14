#include "Graphics/Asset/MeshAsset.h"

#include "Graphics/RenderModule.h"

#include "Core/CoreMin.h"
#include "Core/Memory/PtrBinding.h"


#include <algorithm>



namespace luna::render
{
RegisterTypeEmbedd_Imp(SubMesh)
{
	cls->Ctor<SubMesh>();
	cls->Property<&Self::mVertexCount>("vertex_size");
	cls->Property<&Self::mIndexCount>("index_size");

	cls->Binding<Self>();
	BindingModule::Get("luna")->AddType(cls);
};

RegisterTypeEmbedd_Imp(MeshAsset)
{
	cls->Binding<Self>();
	BindingModule::Get("luna")->AddType(cls);
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

	mVeretexLayout.AddVertexElement(VertexElementType::Float, VertexElementUsage::UsagePosition, 3, 0, VertexElementInstanceType::PerVertex);
	mVeretexLayout.AddVertexElement(VertexElementType::Float, VertexElementUsage::UsageColor, 4, 0, VertexElementInstanceType::PerVertex);
	mVeretexLayout.AddVertexElement(VertexElementType::Float, VertexElementUsage::UsageNormal, 3, 0, VertexElementInstanceType::PerVertex);
	mVeretexLayout.AddVertexElement(VertexElementType::Float, VertexElementUsage::UsageTangent, 4, 0, VertexElementInstanceType::PerVertex);

	mVeretexLayout.AddVertexElement(VertexElementType::Float, VertexElementUsage::UsageTexture0, 2, 0, VertexElementInstanceType::PerVertex);
	mVeretexLayout.AddVertexElement(VertexElementType::Float, VertexElementUsage::UsageTexture1, 2, 0, VertexElementInstanceType::PerVertex);
	mVeretexLayout.AddVertexElement(VertexElementType::Float, VertexElementUsage::UsageTexture2, 2, 0, VertexElementInstanceType::PerVertex);
	mVeretexLayout.AddVertexElement(VertexElementType::Float, VertexElementUsage::UsageTexture3, 2, 0, VertexElementInstanceType::PerVertex);

	mVeretexLayout.AddVertexElement(VertexElementType::Int, VertexElementUsage::UsageInstanceMessage, 4, 1, VertexElementInstanceType::PerInstance);

	Update();
}

void MeshAsset::OnAssetFileRead(LSharedPtr<Dictionary> meta, LSharedPtr<LFile> file)
{
	const byte* data_header = file->GetData().data();
	size_t offset = 0;
	const byte* ptr = data_header;
	size_t submeshSize;
	memcpy(&submeshSize, ptr, sizeof(size_t));
	for (size_t id = 0; id < submeshSize; ++id)
	{
		render::SubMesh* sub_mesh = TCreateObject<render::SubMesh>();
		size_t submeshVertexSize;
		size_t submeshIndexSize;
		ptr += sizeof(size_t);
		memcpy(&submeshVertexSize, ptr, sizeof(size_t));
		ptr += sizeof(size_t);
		memcpy(&submeshIndexSize, ptr, sizeof(size_t));
		sub_mesh->mVertexCount = submeshVertexSize;
		sub_mesh->mVertexData.resize(submeshVertexSize);
		sub_mesh->mIndexCount = submeshVertexSize;
		sub_mesh->mIndexData.resize(submeshIndexSize);
		mSubMesh.PushBack(sub_mesh);
	}

	for (size_t idx = 0; idx < mSubMesh.Size(); ++idx)
	{
		memcpy(mSubMesh[idx]->mVertexData.data(), ptr, mSubMesh[idx]->mVertexData.size() * sizeof(BaseVertex));
		ptr += mSubMesh[idx]->mVertexData.size() * sizeof(BaseVertex);
		memcpy(mSubMesh[idx]->mIndexData.data(), ptr, mSubMesh[idx]->mIndexData.size() * sizeof(uint32_t));
		ptr += mSubMesh[idx]->mIndexData.size() * sizeof(uint32_t);
	}
	OnLoad();
}

void MeshAsset::OnAssetFileWrite(LSharedPtr<Dictionary> meta, LVector<byte>& data)
{
	size_t globel_size = 0;
	size_t offset = 0;
	globel_size += sizeof(size_t);
	for (size_t id = 0; id < mSubMesh.Size(); ++id)
	{
		globel_size += 2 * sizeof(size_t);
		globel_size += mSubMesh[id]->mVertexData.size() * sizeof(BaseVertex);
		globel_size += mSubMesh[id]->mIndexData.size() * sizeof(uint32_t);
	}
	data.resize(globel_size);
	byte* dst = data.data();
	size_t submeshSize = mSubMesh.Size();
	memcpy(dst, &submeshSize, sizeof(size_t));
	dst += sizeof(size_t);
	for(size_t submeshIndex = 0; submeshIndex < mSubMesh.Size(); ++submeshIndex)
	{
		size_t submeshVertexSize = mSubMesh[submeshIndex]->mVertexData.size();
		size_t submeshIndexSize = mSubMesh[submeshIndex]->mIndexData.size();
		memcpy(dst, &submeshVertexSize, sizeof(size_t));
		dst += sizeof(size_t);
		memcpy(dst, &submeshIndexSize, sizeof(size_t));
		dst += sizeof(size_t);
	}

	for (size_t idx = 0; idx < mSubMesh.Size(); ++idx)
	{
		memcpy(dst, mSubMesh[idx]->mVertexData.data(), mSubMesh[idx]->mVertexData.size() * sizeof(BaseVertex));
		dst += mSubMesh[idx]->mVertexData.size() * sizeof(BaseVertex);
		memcpy(dst, mSubMesh[idx]->mIndexData.data(), mSubMesh[idx]->mIndexData.size() * sizeof(uint32_t));
		dst += mSubMesh[idx]->mIndexData.size() * sizeof(uint32_t);
	}
}


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

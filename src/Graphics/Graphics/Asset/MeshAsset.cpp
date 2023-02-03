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
	cls->Binding<Self>();
	BindingModule::Get("luna")->AddType(cls);
};

RegisterTypeEmbedd_Imp(MeshAsset)
{
	cls->Binding<Self>();
	BindingModule::Get("luna")->AddType(cls);
	cls->Ctor<MeshAsset>();
};

void SubMesh::Release()
{
	mReady = false;
}

void SubMesh::Update()
{
	if (mReady)
		return;
	if (mVertexData.size() == 0 || mIndexData.size() == 0)
		return;
	RHIBufferDesc desc;
	desc.mSize = sizeof(BaseVertex) * mVertexData.size();
	desc.mBufferUsage = RHIBufferUsage::VertexBufferBit;

	mVB = sRenderModule->mRenderContext->CreateBuffer(desc, mVertexData.data());

	desc.mSize = sizeof(uint32_t) * mIndexData.size();
	desc.mBufferUsage = RHIBufferUsage::IndexBufferBit;
	mIB = sRenderModule->mRenderContext->CreateBuffer( desc, mIndexData.data());
	mReady = true;
}

void SubMesh::ClearVertexData()
{
	mVertexData.clear();
	mIndexData.clear();
}

void SubMesh::AddLine(const BaseVertex& v1, const BaseVertex& v2)
{
	mIndexData.push_back(mVertexData.size());
	mVertexData.push_back(v1);
	mIndexData.push_back(mVertexData.size());
	mVertexData.push_back(v2);
	mReady = false;
}

void SubMesh::AddTriangle(const BaseVertex& v1, const BaseVertex& v2, const BaseVertex& v3)
{
	mIndexData.push_back(mVertexData.size());
	mVertexData.push_back(v1);
	mIndexData.push_back(mVertexData.size());
	mVertexData.push_back(v2);
	mIndexData.push_back(mVertexData.size());
	mVertexData.push_back(v3);
	mReady = false;
}

void SubMesh::SetVertexFormat(const RHIVertexLayout& format)
{
	mVeretexLayout = format;
	mReady = false;
}

void SubMesh::AddCubeWired(const LVector3f& position, const LVector3f& size /*= LVector3f(1, 1, 1)*/, const LVector4f& color /*= LVector4f(1, 1, 1)*/)
{
	BaseVertex v1;
	v1.color = color;
	v1.pos = position;
	float x, y, z;
	x = size.x();
	y = size.y();
	z = size.z();
	v1.pos.x() -= x * 0.5f;
	v1.pos.y() -= y * 0.5f;
	v1.pos.z() -= z * 0.5;
	BaseVertex v2 = v1;
	v2.pos.x() += x;
	AddLine(v1, v2);
	BaseVertex v3 = v1;
	v3.pos.z() += z;
	AddLine(v1, v3);
	BaseVertex v4 = v1;
	v4.pos.x() += x;
	v4.pos.z() += z;
	AddLine(v2, v4);
	AddLine(v3, v4);
	BaseVertex v5 = v1;
	BaseVertex v6 = v2;
	BaseVertex v7 = v3;
	BaseVertex v8 = v4;
	v5.pos.y() += y;
	v6.pos.y() += y;
	v7.pos.y() += y;
	v8.pos.y() += y;
	AddLine(v5, v6);
	AddLine(v5, v7);
	AddLine(v6, v8);
	AddLine(v7, v8);

	AddLine(v1, v5);
	AddLine(v2, v6);
	AddLine(v3, v7);
	AddLine(v4, v8);

}

SubMesh::SubMesh()
{
	mVeretexLayout = BaseVertex::GetVertexLayout();
}

void MeshAsset::OnAssetFileRead(LSharedPtr<JsonDict> meta, LSharedPtr<LFile> file)
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
		ptr += sizeof(size_t);
		sub_mesh->mVertexData.resize(submeshVertexSize);
		sub_mesh->mIndexData.resize(submeshIndexSize);
		mSubMesh.push_back(sub_mesh);
	}

	for (size_t idx = 0; idx < mSubMesh.size(); ++idx)
	{
		luna::render::SubMesh* subMeshData = mSubMesh[idx];
		memcpy(subMeshData->mVertexData.data(), ptr, subMeshData->mVertexData.size() * sizeof(BaseVertex));
		ptr += subMeshData->mVertexData.size() * sizeof(BaseVertex);
		memcpy(subMeshData->mIndexData.data(), ptr, subMeshData->mIndexData.size() * sizeof(uint32_t));
		ptr += subMeshData->mIndexData.size() * sizeof(uint32_t);
		
		subMeshData->GetVertexLayout().AddVertexElement(VertexElementType::Int, VertexElementUsage::UsageInstanceMessage, 4, 1, VertexElementInstanceType::PerInstance);
		subMeshData->Update();
	}
	OnLoad();
};

void MeshAsset::OnAssetFileWrite(LSharedPtr<JsonDict> meta, LArray<byte>& data)
{
	size_t globel_size = 0;
	size_t offset = 0;
	globel_size += sizeof(size_t);
	for (size_t id = 0; id < mSubMesh.size(); ++id)
	{
		luna::render::SubMesh* subMeshData = mSubMesh[id];
		globel_size += 2 * sizeof(size_t);
		globel_size += subMeshData->mVertexData.size() * sizeof(BaseVertex);
		globel_size += subMeshData->mIndexData.size() * sizeof(uint32_t);
	}
	data.resize(globel_size);
	byte* dst = data.data();
	size_t submeshSize = mSubMesh.size();
	memcpy(dst, &submeshSize, sizeof(size_t));
	dst += sizeof(size_t);
	for (size_t submeshIndex = 0; submeshIndex < mSubMesh.size(); ++submeshIndex)
	{
		luna::render::SubMesh* subMeshData = mSubMesh[submeshIndex];
		size_t submeshVertexSize = subMeshData->mVertexData.size();
		size_t submeshIndexSize = subMeshData->mIndexData.size();
		memcpy(dst, &submeshVertexSize, sizeof(size_t));
		dst += sizeof(size_t);
		memcpy(dst, &submeshIndexSize, sizeof(size_t));
		dst += sizeof(size_t);
	}

	for (size_t idx = 0; idx < mSubMesh.size(); ++idx)
	{
		luna::render::SubMesh* subMeshData = mSubMesh[idx];
		memcpy(dst, subMeshData->mVertexData.data(), subMeshData->mVertexData.size() * sizeof(BaseVertex));
		dst += subMeshData->mVertexData.size() * sizeof(BaseVertex);
		memcpy(dst, subMeshData->mIndexData.data(), subMeshData->mIndexData.size() * sizeof(uint32_t));
		dst += subMeshData->mIndexData.size() * sizeof(uint32_t);
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

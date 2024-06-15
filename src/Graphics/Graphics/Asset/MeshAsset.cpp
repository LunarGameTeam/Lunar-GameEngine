#include "Graphics/Asset/MeshAsset.h"
#include "Core/CoreMin.h"
#include "Core/Memory/PtrBinding.h"
#include "Graphics/Renderer/RenderMesh.h"
#include "Graphics/Renderer/RenderContext.h"

#include <algorithm>

namespace luna::graphics
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

	void SubMesh::ClearVertexData()
	{
		mVertexData.clear();
		mIndexData.clear();
	}

	void SubMesh::SetVertexFormat(const RHIVertexLayout& format)
	{
		mVeretexLayout = format;
	}

	graphics::RenderAssetDataMesh* SubMesh::GetRenderMeshData()
	{
		if(!mMeshData)
		{
			mMeshData = sRenderResourceContext->GetAssetManager()->GenerateRenderMesh(this);
		}
		return mMeshData;
	}
	
	SubMesh::~SubMesh()
	{
		if (mMeshData)
		{
			sRenderResourceContext->GetAssetManager()->DestroyRenderMesh(mMeshData);
		}
	}

	SubMesh::SubMesh()
	{
		mVeretexLayout = BaseVertex::GetVertexLayout();
		mType = SubMeshType::SubMeshStatic;
	}

	void MeshAsset::OnAssetFileRead(LSharedPtr<JsonDict> meta, LSharedPtr<LFile> file)
	{
		const byte* data_header = file->GetData().data();
		size_t offset = 0;
		const byte* ptr = data_header;
		size_t submeshSize;
		memcpy(&submeshSize, ptr, sizeof(size_t));
		ptr += sizeof(size_t);
		for (size_t id = 0; id < submeshSize; ++id)
		{
			size_t submeshVertexSize;
			size_t submeshIndexSize;
			memcpy(&submeshVertexSize, ptr, sizeof(size_t));
			ptr += sizeof(size_t);
			memcpy(&submeshIndexSize, ptr, sizeof(size_t));
			ptr += sizeof(size_t);
			graphics::SubMesh* sub_mesh = GenerateSubmesh(submeshVertexSize, submeshIndexSize);

			sub_mesh->mAssetPath = GetAssetPath();
			sub_mesh->mSubmeshIndex = id;
			mSubMesh.push_back(sub_mesh);
		}

		for (size_t idx = 0; idx < mSubMesh.size(); ++idx)
		{
			ReadVertexData(idx, ptr);
		}
		OnLoad();
	};

	void MeshAsset::OnAssetFileWrite(LSharedPtr<JsonDict> meta, LArray<byte>& data)
	{
		size_t submeshSize = mSubMesh.size();
		CopyPointToByteArray(&submeshSize, sizeof(size_t), data);
		for (size_t submeshIndex = 0; submeshIndex < mSubMesh.size(); ++submeshIndex)
		{
			luna::graphics::SubMesh* subMeshData = mSubMesh[submeshIndex];
			size_t submeshVertexSize = subMeshData->mVertexData.size();
			size_t submeshIndexSize = subMeshData->mIndexData.size();
			CopyPointToByteArray(&submeshVertexSize, sizeof(size_t), data);
			CopyPointToByteArray(&submeshIndexSize, sizeof(size_t), data);
		}

		for (size_t idx = 0; idx < mSubMesh.size(); ++idx)
		{
			WriteVertexData(idx, data);
		}
	}

	void MeshAsset::ReadVertexData(size_t idx, const byte* &ptr)
	{
		luna::graphics::SubMesh* subMeshData = mSubMesh[idx];
		memcpy(subMeshData->mVertexData.data(), ptr, subMeshData->mVertexData.size() * sizeof(BaseVertex));
		ptr += subMeshData->mVertexData.size() * sizeof(BaseVertex);
		memcpy(subMeshData->mIndexData.data(), ptr, subMeshData->mIndexData.size() * sizeof(uint32_t));
		ptr += subMeshData->mIndexData.size() * sizeof(uint32_t);
	}

	void MeshAsset::WriteVertexData(size_t idx, LArray<byte>& data)
	{
		luna::graphics::SubMesh* subMeshData = mSubMesh[idx];
		CopyPointToByteArray(subMeshData->mVertexData.data(), subMeshData->mVertexData.size() * sizeof(BaseVertex),data);
		CopyPointToByteArray(subMeshData->mIndexData.data(), subMeshData->mIndexData.size() * sizeof(uint32_t), data);
	}

	graphics::SubMesh* MeshAsset::GenerateSubmesh(size_t submeshVertexSize, size_t submeshIndexSize)
	{
		graphics::SubMesh* sub_mesh = TCreateObject<graphics::SubMesh>();
		sub_mesh->mVertexData.resize(submeshVertexSize);
		sub_mesh->mIndexData.resize(submeshIndexSize);
		return sub_mesh;
	}
}

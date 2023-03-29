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

	void SubMesh::ClearVertexData()
	{
		mVertexData.clear();
		mIndexData.clear();
	}

	void SubMesh::SetVertexFormat(const RHIVertexLayout& format)
	{
		mVeretexLayout = format;
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
		ptr += sizeof(size_t);
		for (size_t id = 0; id < submeshSize; ++id)
		{
			size_t submeshVertexSize;
			size_t submeshIndexSize;
			memcpy(&submeshVertexSize, ptr, sizeof(size_t));
			ptr += sizeof(size_t);
			memcpy(&submeshIndexSize, ptr, sizeof(size_t));
			ptr += sizeof(size_t);
			render::SubMesh* sub_mesh = GenerateSubmesh(submeshVertexSize, submeshIndexSize);

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
			WriteVertexData(idx, dst);
		}
	}

	void MeshAsset::ReadVertexData(size_t idx, const byte* ptr)
	{
		luna::render::SubMesh* subMeshData = mSubMesh[idx];
		memcpy(subMeshData->mVertexData.data(), ptr, subMeshData->mVertexData.size() * sizeof(BaseVertex));
		ptr += subMeshData->mVertexData.size() * sizeof(BaseVertex);
		memcpy(subMeshData->mIndexData.data(), ptr, subMeshData->mIndexData.size() * sizeof(uint32_t));
		ptr += subMeshData->mIndexData.size() * sizeof(uint32_t);
	}

	void MeshAsset::WriteVertexData(size_t idx, byte* dst)
	{
		luna::render::SubMesh* subMeshData = mSubMesh[idx];
		memcpy(dst, subMeshData->mVertexData.data(), subMeshData->mVertexData.size() * sizeof(BaseVertex));
		dst += subMeshData->mVertexData.size() * sizeof(BaseVertex);
		memcpy(dst, subMeshData->mIndexData.data(), subMeshData->mIndexData.size() * sizeof(uint32_t));
		dst += subMeshData->mIndexData.size() * sizeof(uint32_t);
	}

	render::SubMesh* MeshAsset::GenerateSubmesh(size_t submeshVertexSize, size_t submeshIndexSize)
	{
		render::SubMesh* sub_mesh = TCreateObject<render::SubMesh>();
		sub_mesh->mVertexData.resize(submeshVertexSize);
		sub_mesh->mIndexData.resize(submeshIndexSize);
		return sub_mesh;
	}

}

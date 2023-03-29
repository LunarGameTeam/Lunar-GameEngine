#include "Graphics/Asset/SkeletalMeshAsset.h"

#include "Graphics/RenderModule.h"

#include "Core/CoreMin.h"
#include "Core/Memory/PtrBinding.h"


#include <algorithm>

namespace luna::render
{
	RegisterTypeEmbedd_Imp(SubMeshSkeletal)
	{
		cls->Ctor<SubMeshSkeletal>();
		cls->Binding<Self>();
		BindingModule::Get("luna")->AddType(cls);
	};

	RegisterTypeEmbedd_Imp(SkeletalMeshAsset)
	{
		cls->Binding<Self>();
		BindingModule::Get("luna")->AddType(cls);
		cls->Ctor<SkeletalMeshAsset>();
	};

	SubMeshSkeletal::SubMeshSkeletal()
	{
		mVeretexLayout = SkinVertex::GetVertexLayout();
	}

	render::SubMesh* SkeletalMeshAsset::GenerateSubmesh(size_t submeshVertexSize, size_t submeshIndexSize)
	{
		render::SubMeshSkeletal* sub_mesh = TCreateObject<render::SubMeshSkeletal>();
		sub_mesh->mVertexData.resize(submeshVertexSize);
		sub_mesh->mIndexData.resize(submeshIndexSize);
		sub_mesh->mSkinData.resize(submeshVertexSize);
		return sub_mesh;
	}

	void SkeletalMeshAsset::ReadVertexData(size_t idx, const byte* ptr)
	{
		luna::render::SubMeshSkeletal* subMeshData = static_cast<SubMeshSkeletal*>(mSubMesh[idx]);
		memcpy(subMeshData->mSkinData.data(), ptr, subMeshData->mSkinData.size() * sizeof(BaseVertex));
		ptr += subMeshData->mSkinData.size() * sizeof(BaseVertex);
	}

	void SkeletalMeshAsset::WriteVertexData(size_t idx, byte* dst)
	{
		luna::render::SubMeshSkeletal* subMeshData = static_cast<SubMeshSkeletal*>(mSubMesh[idx]);
		memcpy(dst, subMeshData->mSkinData.data(), subMeshData->mSkinData.size() * sizeof(BaseVertex));
		dst += subMeshData->mSkinData.size() * sizeof(BaseVertex);
	}
}

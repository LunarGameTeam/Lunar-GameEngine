#include "Graphics/Asset/SkeletalMeshAsset.h"

#include "Graphics/RenderModule.h"

#include "Core/CoreMin.h"
#include "Core/Memory/PtrBinding.h"


#include <algorithm>

namespace luna::graphics
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
		mType = SubMeshType::SubMeshSkined;
	}

	graphics::SubMesh* SkeletalMeshAsset::GenerateSubmesh(size_t submeshVertexSize, size_t submeshIndexSize)
	{
		graphics::SubMeshSkeletal* sub_mesh = TCreateObject<graphics::SubMeshSkeletal>();
		sub_mesh->mVertexData.resize(submeshVertexSize);
		sub_mesh->mIndexData.resize(submeshIndexSize);
		sub_mesh->mSkinData.resize(submeshVertexSize);
		return sub_mesh;
	}

	void SkeletalMeshAsset::ReadVertexData(size_t idx, const byte* &ptr)
	{
		MeshAsset::ReadVertexData(idx, ptr);
		luna::graphics::SubMeshSkeletal* subMeshData = static_cast<SubMeshSkeletal*>(mSubMesh[idx]);
		memcpy(subMeshData->mSkinData.data(), ptr, subMeshData->mSkinData.size() * sizeof(SkinVertex));
		ptr += subMeshData->mSkinData.size() * sizeof(SkinVertex);
		size_t refBoneNumber = 0;
		memcpy(&refBoneNumber, ptr, sizeof(size_t));
		ptr += sizeof(size_t);
		for (int32_t boneIndex = 0; boneIndex < refBoneNumber; ++boneIndex)
		{
			size_t nameLength = 0;
			memcpy(&nameLength, ptr, sizeof(size_t));
			ptr += sizeof(size_t);
			char* namePtr = new char[nameLength + 1];
			namePtr[nameLength] = '\0';
			memcpy(namePtr, ptr, nameLength * sizeof(char));
			subMeshData->mRefBoneName.push_back(namePtr);
			delete[] namePtr;
			ptr += nameLength * sizeof(char);
		}
		subMeshData->mRefBonePose.resize(refBoneNumber);
		memcpy(subMeshData->mRefBonePose.data(), ptr, refBoneNumber * sizeof(luna::LMatrix4f));
		ptr += refBoneNumber * sizeof(luna::LMatrix4f);
	}

	void SkeletalMeshAsset::WriteVertexData(size_t idx, LArray<byte>& data)
	{
		MeshAsset::WriteVertexData(idx, data);
		luna::graphics::SubMeshSkeletal* subMeshData = static_cast<SubMeshSkeletal*>(mSubMesh[idx]);
		CopyPointToByteArray(subMeshData->mSkinData.data(), subMeshData->mSkinData.size() * sizeof(SkinVertex), data);
		size_t refBoneNumber = subMeshData->mRefBoneName.size();
		CopyPointToByteArray(&refBoneNumber, sizeof(size_t), data);
		for (int32_t boneIndex = 0; boneIndex < subMeshData->mRefBoneName.size(); ++boneIndex)
		{
			LString &eachBoneName = subMeshData->mRefBoneName[boneIndex];
			size_t nameLength = eachBoneName.Length();
			CopyPointToByteArray(&nameLength, sizeof(size_t), data);
			CopyPointToByteArray(eachBoneName.c_str(), eachBoneName.Length() * sizeof(char), data);
		}
		CopyPointToByteArray(subMeshData->mRefBonePose.data(), subMeshData->mRefBonePose.size() * sizeof(luna::LMatrix4f), data);
	}
}

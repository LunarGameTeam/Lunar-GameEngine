#pragma once
#include "AssetImport/ImportScene/ImportDataMesh.h"
#include "AssetImport/ImportScene/ThirdParty/mikktspace.h"
#include "AssetImport/ImportScene/ThirdParty/forsythtriangleorderoptimizer.h"

namespace luna::asset
{
	LImportNodeDataMesh::LImportNodeDataMesh(const size_t index) :LImportNodeDataBase(LImportNodeDataType::ImportDataMesh, index)
	{

	};

	void LImportNodeDataMesh::AddFullVertex(
		const size_t subMeshIndex,
		const LVector3f pos,
		const LVector3f norm,
		const LVector4f tangent,
		const LArray<LVector2f> uv,
		const LVector4f color
	)
	{
		mSubmesh[subMeshIndex].mVertexPosition.push_back(pos);
		mSubmesh[subMeshIndex].mVertexNormal.push_back(norm);
		mSubmesh[subMeshIndex].mVertexTangent.push_back(tangent);
		mSubmesh[subMeshIndex].mVertexUv.push_back(uv);
		mSubmesh[subMeshIndex].mVertexColor.push_back(color);
	};

	void LImportNodeDataMesh::AddFullSkin(
		const size_t subMeshIndex,
		const LArray<uint32_t>& mRefBone,
		const LArray<float>& mWeight
	)
	{
		int32_t addCount = (int32_t)mRefBone.size();
		if (!mHasSkin && addCount > 0)
		{
			mHasSkin = true;
		}
		for (int32_t i = 0; i < addCount; ++i)
		{
			mSubmesh[subMeshIndex].mRefBone.push_back(mRefBone[i]);
			mSubmesh[subMeshIndex].mWeight.push_back(mWeight[i]);
		}
		for (int32_t i = addCount; i < asset::gSkinPerVertex; ++i)
		{
			mSubmesh[subMeshIndex].mRefBone.push_back(0);
			mSubmesh[subMeshIndex].mWeight.push_back(0);
		}
	}

	size_t LImportNodeDataMesh::AddSubMeshMessage(
		const LString& name,
		const size_t materialUse
	)
	{
		LImportSubmesh newSubmesh;
		newSubmesh.mName = name;
		newSubmesh.mIndex = mSubmesh.size();
		newSubmesh.mMaterialUse = materialUse;
		mSubmesh.push_back(newSubmesh);
		return newSubmesh.mIndex;
	}

	void LImportNodeDataMesh::AddFaceIndexDataToSubmesh(const size_t subMeshIndex, const uint32_t index)
	{
		if (subMeshIndex >= mSubmesh.size())
		{
			return;
		};
		mSubmesh[subMeshIndex].mIndices.push_back(index);
	}

	void LImportNodeDataMesh::AddBoneMessageToSubmesh(const size_t subMeshIndex, const LString& boneName, const LMatrix4f& bonePose)
	{
		mSubmesh[subMeshIndex].mRefBoneName.push_back(boneName);
		mSubmesh[subMeshIndex].mRefBonePose.push_back(bonePose);
	}

	void LImportNodeDataMesh::ConvertDataAxisAndUnitImpl(bool hasReflectTransform, LMatrix4f convertInvMatrix, LMatrix4f convertMatrix)
	{
		for (size_t submeshID = 0; submeshID < mSubmesh.size(); ++submeshID)
		{
			LImportSubmesh& subMeshData = mSubmesh[submeshID];
			for (size_t vertexID = 0; vertexID < subMeshData.mVertexPosition.size(); ++vertexID)
			{
				//变换position数据
				LTransform translationMid;
				translationMid.setIdentity();
				translationMid.translate(subMeshData.mVertexPosition[vertexID]);
				LMatrix4f transDataAns = convertInvMatrix * translationMid.matrix() * convertMatrix;
				subMeshData.mVertexPosition[vertexID] = transDataAns.block<3, 1>(0, 3);
				//变换normal数据
				translationMid.setIdentity();
				translationMid.translate(subMeshData.mVertexNormal[vertexID]);
				transDataAns = convertInvMatrix * translationMid.matrix() * convertMatrix;
				subMeshData.mVertexNormal[vertexID] = transDataAns.block<3, 1>(0, 3);
				subMeshData.mVertexNormal[vertexID].normalize();
				//变换tangent数据
				LVector3f tangentValue = subMeshData.mVertexTangent[vertexID].head<3>();
				translationMid.setIdentity();
				translationMid.translate(tangentValue);
				transDataAns = convertInvMatrix * translationMid.matrix() * convertMatrix;
				tangentValue = transDataAns.block<3, 1>(0, 3);
				tangentValue.normalize();
				float bnormalSign = subMeshData.mVertexTangent[vertexID].w();
				subMeshData.mVertexTangent[vertexID] = LVector4f(tangentValue.x(), tangentValue.y(), tangentValue.z(), bnormalSign);
			}
			if (hasReflectTransform)
			{
				for (size_t faceID = 0; faceID < subMeshData.mIndices.size() / 3; ++faceID)
				{
					uint32_t mid_index = subMeshData.mIndices[faceID * 3];
					subMeshData.mIndices[faceID * 3] = subMeshData.mIndices[faceID * 3 + 2];
					subMeshData.mIndices[faceID * 3 + 2] = mid_index;
				}
			}
			for (int32_t boneId = 0; boneId < subMeshData.mRefBonePose.size(); ++boneId)
			{
				LMatrix4f refMatMid;
				refMatMid = convertInvMatrix * subMeshData.mRefBonePose[boneId] * convertMatrix;
				subMeshData.mRefBonePose[boneId] = refMatMid;
			}
		}
	}

	static int MikkGetNumFaces(const SMikkTSpaceContext* Context)
	{
		LImportSubmesh* UserData = (LImportSubmesh*)(Context->m_pUserData);
		return UserData->mIndices.size() / 3;
	}

	static int MikkGetNumVertsOfFace(const SMikkTSpaceContext* Context, const int FaceIdx)
	{
		// All of our meshes are triangles.
		return 3;
	}

	static void MikkGetPosition(const SMikkTSpaceContext* Context, float Position[3], const int FaceIdx, const int VertIdx)
	{
		LImportSubmesh* UserData = (LImportSubmesh*)(Context->m_pUserData);
		const LVector3f& VertexPosition = UserData->mVertexPosition[UserData->mIndices[FaceIdx * 3 + VertIdx]];
		Position[0] = VertexPosition.x();
		Position[1] = VertexPosition.y();
		Position[2] = VertexPosition.z();
	}

	static void MikkGetNormal(const SMikkTSpaceContext* Context, float Normal[3], const int FaceIdx, const int VertIdx)
	{
		LImportSubmesh* UserData = (LImportSubmesh*)(Context->m_pUserData);
		const LVector3f& VertexNormal = UserData->mVertexNormal[UserData->mIndices[FaceIdx * 3 + VertIdx]];
		Normal[0] = VertexNormal.x();
		Normal[1] = VertexNormal.y();
		Normal[2] = VertexNormal.z();
	}

	static void MikkSetTSpaceBasic(const SMikkTSpaceContext* Context, const float Tangent[3], const float BitangentSign, const int FaceIdx, const int VertIdx)
	{
		LImportSubmesh* UserData = (LImportSubmesh*)(Context->m_pUserData);
		LVector4f& VertexTangent = UserData->mVertexTangent[UserData->mIndices[FaceIdx * 3 + VertIdx]];
		VertexTangent.x() = Tangent[0];
		VertexTangent.y() = Tangent[1];
		VertexTangent.z() = Tangent[2];
		VertexTangent.w() = BitangentSign;
	}

	static void MikkGetTexCoord(const SMikkTSpaceContext* Context, float UV[2], const int FaceIdx, const int VertIdx)
	{
		LImportSubmesh* UserData = (LImportSubmesh*)(Context->m_pUserData);
		const LVector2f& TexCoord = UserData->mVertexUv[UserData->mIndices[FaceIdx * 3 + VertIdx]][0];
		UV[0] = TexCoord.x();
		UV[1] = TexCoord.y();
	}

	void LImportNodeDataMesh::GenerateMikkTspaceTangent()
	{
		for (size_t subMeshIndex = 0; subMeshIndex < mSubmesh.size(); ++subMeshIndex)
		{
			LImportSubmesh& submeshData = mSubmesh[subMeshIndex];
			if (submeshData.mVertexUv[0].empty())
			{
				continue;
			}
			SMikkTSpaceInterface MikkTInterface;
			MikkTInterface.m_getNormal = MikkGetNormal;
			MikkTInterface.m_getNumFaces = MikkGetNumFaces;
			MikkTInterface.m_getNumVerticesOfFace = MikkGetNumVertsOfFace;
			MikkTInterface.m_getPosition = MikkGetPosition;
			MikkTInterface.m_getTexCoord = MikkGetTexCoord;
			MikkTInterface.m_setTSpaceBasic = MikkSetTSpaceBasic;
			MikkTInterface.m_setTSpace = nullptr;

			SMikkTSpaceContext MikkTContext;
			MikkTContext.m_pInterface = &MikkTInterface;
			MikkTContext.m_pUserData = (void*)(&submeshData);
			genTangSpaceDefault(&MikkTContext);
		}

	}

	void LImportNodeDataMesh::ComputeVertexCache()
	{
		for (size_t subMeshIndex = 0; subMeshIndex < mSubmesh.size(); ++subMeshIndex)
		{
			LImportSubmesh& submeshData = mSubmesh[subMeshIndex];
			int32_t original_index_size = submeshData.mIndices.size();
			int32_t original_vertex_size = submeshData.mVertexPosition.size();
			uint32_t* chunk_index_list = new uint32_t[original_index_size];
			//调用第三方库整理索引缓冲区的顺序
			Forsyth::OptimizeFaces(submeshData.mIndices.data(), original_index_size, original_vertex_size, chunk_index_list, (uint16_t)32);
			//记录旧的顶点顺序
			LImportSubmesh original_vertex_list;
			CopyAndClearSubmeshVertex(submeshData, original_vertex_list);
			//创建一个记录新顶点顺序的索引表
			LArray<int32_t> cache_index_list;
			cache_index_list.resize(original_vertex_size);
			for (int32_t i = 0; i < original_vertex_size; ++i)
			{
				cache_index_list[i] = -1;
			}
			//调整顶点顺序
			int32_t NextAvailableIndex = 0;
			for (int32_t Index = 0; Index < original_index_size; Index++)
			{
				const int32_t original_index = chunk_index_list[Index];
				const int32_t cache_index = cache_index_list[original_index];
				if (cache_index == -1)
				{
					chunk_index_list[Index] = NextAvailableIndex;
					cache_index_list[original_index] = NextAvailableIndex;
					NextAvailableIndex++;
				}
				else
				{
					chunk_index_list[Index] = cache_index;
				}
				CopyVertexData(original_vertex_list, submeshData, original_index, chunk_index_list[Index]);
			}
			for (int32_t Index = 0; Index < original_index_size; Index++)
			{
				submeshData.mIndices[Index] = chunk_index_list[Index];
			}
			delete[] chunk_index_list;
		}
	}
	
	void LImportNodeDataMesh::CopyAndClearSubmeshVertex(LImportSubmesh& src, LImportSubmesh& dst)
	{
		size_t vertexSize = src.mVertexPosition.size();
		dst.mVertexPosition.resize(vertexSize);
		std::swap(src.mVertexPosition,dst.mVertexPosition);
		dst.mVertexNormal.resize(vertexSize);
		std::swap(src.mVertexNormal, dst.mVertexNormal);
		dst.mVertexTangent.resize(vertexSize);
		std::swap(src.mVertexTangent, dst.mVertexTangent);
		dst.mVertexColor.resize(vertexSize);
		std::swap(src.mVertexColor, dst.mVertexColor);
		dst.mVertexUv.resize(vertexSize);
		for (size_t vertexID = 0; vertexID < vertexSize; ++vertexID)
		{
			size_t uvChannelSize = src.mVertexUv[vertexID].size();
			dst.mVertexUv[vertexID].resize(uvChannelSize);
			std::swap(src.mVertexUv[vertexID], dst.mVertexUv[vertexID]);
		}
		if(mHasSkin)
		{
			dst.mRefBone.resize(vertexSize * asset::gSkinPerVertex);
			std::swap(src.mRefBone, dst.mRefBone);
			dst.mWeight.resize(vertexSize * asset::gSkinPerVertex);
			std::swap(src.mWeight, dst.mWeight);
		}
	}

	void LImportNodeDataMesh::CopyVertexData(LImportSubmesh& src, LImportSubmesh& dst, size_t srcId, size_t dstId)
	{
		dst.mVertexPosition[dstId] = src.mVertexPosition[srcId];
		dst.mVertexNormal[dstId] = src.mVertexNormal[srcId];
		dst.mVertexTangent[dstId] = src.mVertexTangent[srcId];
		dst.mVertexColor[dstId] = src.mVertexColor[srcId];
		size_t uvChannelSize = src.mVertexUv[srcId].size();
		for (size_t uvChannel = 0; uvChannel < uvChannelSize; ++uvChannel)
		{
			dst.mVertexUv[dstId][uvChannel] = src.mVertexUv[srcId][uvChannel];
		}
		if (mHasSkin)
		{
			for (int32_t refIndex = 0; refIndex < asset::gSkinPerVertex; ++refIndex)
			{
				dst.mRefBone[dstId * asset::gSkinPerVertex + refIndex] = src.mRefBone[srcId * asset::gSkinPerVertex + refIndex];
				dst.mWeight[dstId * asset::gSkinPerVertex + refIndex] = src.mWeight[srcId * asset::gSkinPerVertex + refIndex];
			}
		}
	}
}


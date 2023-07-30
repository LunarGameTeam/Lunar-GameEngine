#include "Graphics/RenderModule.h"
#include "Graphics/Asset/MeshAsset.h"
#include "Graphics/Asset/SkeletalMeshAsset.h"
#include "Graphics/Renderer/SkeletonSkin.h"

namespace  luna::graphics
{

SkeletonSkinData::SkeletonSkinData()
{
	RHIBufferDesc desc;
	desc.mBufferUsage = RHIBufferUsage::StructureBuffer;
	desc.mSize = sizeof(LMatrix4f) * 128;
	mSkeletonResultBuffer = sRenderModule->mRenderContext->CreateBuffer(desc);
	ViewDesc viewDesc;
	viewDesc.mViewType = RHIViewType::kStructuredBuffer;
	viewDesc.mViewDimension = RHIViewDimension::BufferView;
	viewDesc.mStructureStride = sizeof(LMatrix4f);
	mSkeletonResultBufferView = sRenderModule->GetRHIDevice()->CreateView(viewDesc);
	mSkeletonResultBufferView->BindResource(mSkeletonResultBuffer);
}

int32_t SkeletonSkinData::AddMeshSkeletonLinkClusterData(SubMesh* meshData, const LUnorderedMap<LString, int32_t>& skeletonId, const LString& skeletonUniqueName)
{
	if (meshData == nullptr || meshData->mType != SubMeshType::SubMeshSkined)
	{
		return -1;
	}
	LString clusterName = GetClusterName(meshData, skeletonUniqueName);
	int32_t clusterIndex = mMeshSkeletonLinkClusterBuffer.CheckDataIdByName(clusterName);
	if (clusterIndex != -1)
	{
		return clusterIndex;
	}
	clusterIndex = mMeshSkeletonLinkClusterBuffer.AddData(clusterName);
	MeshSkeletonLinkClusterBase* newClusterData = mMeshSkeletonLinkClusterBuffer.GetData(clusterIndex);

	SubMeshSkeletal* meshSkeletalData = static_cast<SubMeshSkeletal*>(meshData);
	int32_t refBoneCount = meshSkeletalData->mRefBoneName.size();
	for (int32_t refBoneId = 0; refBoneId < refBoneCount; ++refBoneId)
	{
		LString& refBoneName = meshSkeletalData->mRefBoneName[refBoneId];
		LMatrix4f& refBonePose = meshSkeletalData->mRefBonePose[refBoneId];
		auto itor = skeletonId.find(refBoneName);
		assert(itor != skeletonId.end());
		int32_t refBoneSkeletonId = itor->second;
		newClusterData->mBindposeMatrix.push_back(refBonePose);
		newClusterData->mSkinBoneIndex2SkeletonBoneIndex.insert({ refBoneId ,refBoneSkeletonId });
	}
	return clusterIndex;
}

int32_t SkeletonSkinData::AddAnimationInstanceMatrixData(const LString& animaInstanceUniqueName, const LArray<LMatrix4f>& allBoneMatrix)
{
	int32_t animInstanceIndex = mAnimationInstanceMatrixBuffer.CheckDataIdByName(animaInstanceUniqueName);
	if (animInstanceIndex != -1)
	{
		return animInstanceIndex;
	}
	animInstanceIndex = mAnimationInstanceMatrixBuffer.AddData(animaInstanceUniqueName);
	AnimationInstanceMatrix* newAnimInstanceData = mAnimationInstanceMatrixBuffer.GetData(animInstanceIndex);
	for (int32_t i = 0; i < allBoneMatrix.size(); ++i)
	{
		newAnimInstanceData->mBoneMatrix.push_back(allBoneMatrix[i]);
	}
	return animInstanceIndex;
}

void SkeletonSkinData::UpdateAnimationInstanceMatrixData(int32_t animInstanceId, const LArray<LMatrix4f>& allBoneMatrix)
{
	AnimationInstanceMatrix* newAnimInstanceData = mAnimationInstanceMatrixBuffer.GetData(animInstanceId);
	for (int32_t i = 0; i < allBoneMatrix.size(); ++i)
	{
		newAnimInstanceData->mBoneMatrix[i] = allBoneMatrix[i];
	}
}

int32_t SkeletonSkinData::GetMeshSkeletonLinkClusterDataId(SubMesh* meshData, const LString& skeletonUniqueName)
{
	return mMeshSkeletonLinkClusterBuffer.CheckDataIdByName(GetClusterName(meshData, skeletonUniqueName));
}

MeshSkeletonLinkClusterBase* SkeletonSkinData::GetMeshSkeletonLinkClusterData(int32_t clusterId)
{
	return mMeshSkeletonLinkClusterBuffer.GetData(clusterId);
}

int32_t SkeletonSkinData::GetAnimationInstanceMatrixDataId(const LString& animaInstanceUniqueName)
{
	return mAnimationInstanceMatrixBuffer.CheckDataIdByName(animaInstanceUniqueName);
}

AnimationInstanceMatrix* SkeletonSkinData::GetAnimationInstanceMatrixData(int32_t animInstanceId)
{
	return mAnimationInstanceMatrixBuffer.GetData(animInstanceId);
}

LString SkeletonSkinData::GetSubmeshName(SubMesh* meshData)
{
	LString primitiveName = meshData->mAssetPath + "_#_" + std::to_string(meshData->mSubmeshIndex);
	return primitiveName;
}

LString SkeletonSkinData::GetClusterName(SubMesh* meshData, const LString& skeletonUniqueName)
{
	return GetSubmeshName(meshData) + "_##_" + skeletonUniqueName;
}

void SkeletonSkinData::PerSceneUpdate(RenderScene* renderScene)
{
	AnimationInstanceMatrix* animationMatrixBuffer = mAnimationInstanceMatrixBuffer.GetData(0);
	MeshSkeletonLinkClusterBase* beginbuffer = mMeshSkeletonLinkClusterBuffer.GetData(0);
	LArray<LMatrix4f> skinMatrixResult;
	if (!beginbuffer)
		return;
	for (int32_t i = 0; i < beginbuffer->mBindposeMatrix.size(); ++i)
	{
		LMatrix4f skinRefMatrix = beginbuffer->mBindposeMatrix[i];
		int32_t animationBoneId = beginbuffer->mSkinBoneIndex2SkeletonBoneIndex[i];
		LMatrix4f animationMatrix = animationMatrixBuffer->mBoneMatrix[animationBoneId];
		skinMatrixResult.push_back(animationMatrix * skinRefMatrix);

	}
	sRenderModule->GetRenderContext()->UpdateConstantBuffer(mSkeletonResultBuffer, skinMatrixResult.data(), skinMatrixResult.size() * sizeof(LMatrix4f));
}


}

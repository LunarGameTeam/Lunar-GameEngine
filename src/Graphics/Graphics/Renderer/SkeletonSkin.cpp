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
	mSkeletonResultBuffer = sRenderModule->mRenderContext->CreateBuffer(RHIHeapType::Upload, desc);
	ViewDesc viewDesc;
	viewDesc.mViewType = RHIViewType::kStructuredBuffer;
	viewDesc.mViewDimension = RHIViewDimension::BufferView;
	viewDesc.mStructureStride = sizeof(LMatrix4f);
	mSkeletonResultBufferView = sRenderModule->GetRHIDevice()->CreateView(viewDesc);
	mSkeletonResultBufferView->BindResource(mSkeletonResultBuffer);
}

void SkeletonSkinData::Create(const LString& skeletonUniqueName, SubMeshSkeletal* meshSkeletalData, const LUnorderedMap<LString, int32_t>& skeletonId)
{
	mSkeletonUniqueName = skeletonUniqueName;
	int32_t refBoneCount = meshSkeletalData->mRefBoneName.size();
	for (int32_t refBoneId = 0; refBoneId < refBoneCount; ++refBoneId)
	{
		LString& refBoneName = meshSkeletalData->mRefBoneName[refBoneId];
		LMatrix4f& refBonePose = meshSkeletalData->mRefBonePose[refBoneId];
		auto itor = skeletonId.find(refBoneName);
		assert(itor != skeletonId.end());
		int32_t refBoneSkeletonId = itor->second;
		mMeshSkeletonLinkClusterBuffer.mBindposeMatrix.push_back(refBonePose);
		mMeshSkeletonLinkClusterBuffer.mSkinBoneIndex2SkeletonBoneIndex.insert({ refBoneId ,refBoneSkeletonId });
	}
}

void SkeletonSkinData::UpdateAnimationInstanceMatrixData(const LArray<LMatrix4f>& allBoneMatrix)
{
	for (int32_t i = 0; i < allBoneMatrix.size(); ++i)
	{
		mAnimationInstanceMatrixBuffer.mBoneMatrix[i] = allBoneMatrix[i];
	}
}

MeshSkeletonLinkClusterBase* SkeletonSkinData::GetMeshSkeletonLinkClusterData(int32_t clusterId)
{
	return &mMeshSkeletonLinkClusterBuffer;
}

AnimationInstanceMatrix* SkeletonSkinData::GetAnimationInstanceMatrixData()
{
	return &mAnimationInstanceMatrixBuffer;
}

void SkeletonSkinData::PerObjectUpdate(RenderObject* renderObject)
{
	LArray<LMatrix4f> skinMatrixResult;
	for (int32_t i = 0; i < mMeshSkeletonLinkClusterBuffer.mBindposeMatrix.size(); ++i)
	{
		LMatrix4f skinRefMatrix = mMeshSkeletonLinkClusterBuffer.mBindposeMatrix[i];
		int32_t animationBoneId = mMeshSkeletonLinkClusterBuffer.mSkinBoneIndex2SkeletonBoneIndex[i];
		LMatrix4f animationMatrix = mAnimationInstanceMatrixBuffer.mBoneMatrix[animationBoneId];
		skinMatrixResult.push_back(animationMatrix * skinRefMatrix);

	}
	sRenderModule->GetRenderContext()->UpdateConstantBuffer(mSkeletonResultBuffer, skinMatrixResult.data(), skinMatrixResult.size() * sizeof(LMatrix4f));
}

void MeshRendererUpdateSkinCommand(
	graphics::RenderScene* curScene,
	RenderMeshBridgeData& renderData,
	const LArray<SubMesh*>& initSubmesh,
	const LString& skeletonUniqueName,
	const LArray<SkeletonBindPoseMatrix>& bindCluster
)
{
	std::function<void(void)> commandFunc = [=, &renderData]()->void {
		RenderObjectDrawData* RoDrawData = curScene->RequireData<RenderObjectDrawData>();
		for (int32_t subMeshIndex = 0; subMeshIndex < renderData.mRenderObjects.size(); ++subMeshIndex)
		{
			SkeletonSkinData* skinData = renderData.mRenderObjects[subMeshIndex]->RequireData<SkeletonSkinData>();
			SubMeshSkeletal* skeletalMeshPointer = static_cast<SubMeshSkeletal*>(initSubmesh[subMeshIndex]);
			skinData->Create(skeletonUniqueName, skeletalMeshPointer, bindCluster[subMeshIndex].mSkeletonId);
			renderData.mSkinData.push_back(skinData);
		}
	};
	curScene->GetRenderDataUpdater()->AddCommand(RenderDataUpdateCommandType::RENDER_DATA_GENERATE, commandFunc);
}

}

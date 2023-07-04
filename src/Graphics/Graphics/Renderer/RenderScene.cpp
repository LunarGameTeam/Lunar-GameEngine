#include "Graphics/Renderer/RenderScene.h"
#include "Graphics/RenderModule.h"

#include "Graphics/Asset/MeshAsset.h"
#include "Graphics/Asset/SkeletalMeshAsset.h"
#include "Graphics/Asset/MeshAssetUtils.h"
#include "Graphics/Renderer/RenderTarget.h"
#include "Graphics/Renderer/MaterialInstance.h"
#include "Graphics/Renderer/MaterialParam.h"
#include "Graphics/Renderer/RenderView.h"

#include "Graphics/FrameGraph/FrameGraph.h"
#include "Core/Asset/AssetModule.h"
#include "Graphics/Asset/ShaderAsset.h"
#include "Graphics/Renderer/RenderLight.h"

namespace luna::render
{

AssetSceneData::AssetSceneData()
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

int32_t AssetSceneData::AddMeshData(SubMesh* meshData)
{
	LString primitiveName = GetSubmeshName(meshData);
	int32_t primitiveIndex = meshPrimitiveBuffer.CheckDataIdByName(primitiveName);
	if (primitiveIndex != -1)
	{
		return primitiveIndex;
	}
	primitiveIndex = meshPrimitiveBuffer.AddData(primitiveName);
	meshPrimitiveBuffer.GetData(primitiveIndex)->Init(meshData);
	return primitiveIndex;
}

int32_t AssetSceneData::AddMeshSkeletonLinkClusterData(SubMesh* meshData, const LUnorderedMap<LString, int32_t>& skeletonId, const LString& skeletonUniqueName)
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

int32_t AssetSceneData::AddAnimationInstanceMatrixData(const LString& animaInstanceUniqueName, const LArray<LMatrix4f>& allBoneMatrix)
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

void AssetSceneData::UpdateAnimationInstanceMatrixData(int32_t animInstanceId, const LArray<LMatrix4f>& allBoneMatrix)
{
	AnimationInstanceMatrix* newAnimInstanceData = mAnimationInstanceMatrixBuffer.GetData(animInstanceId);
	for (int32_t i = 0; i < allBoneMatrix.size(); ++i)
	{
		newAnimInstanceData->mBoneMatrix[i] = allBoneMatrix[i];
	}
}

int32_t AssetSceneData::GetMeshDataId(SubMesh* meshData)
{
	return meshPrimitiveBuffer.CheckDataIdByName(GetSubmeshName(meshData));
}

RenderMeshBase* AssetSceneData::GetMeshData(int32_t meshId)
{
	return meshPrimitiveBuffer.GetData(meshId);
}

int32_t AssetSceneData::GetMeshSkeletonLinkClusterDataId(SubMesh* meshData, const LString& skeletonUniqueName)
{
	return mMeshSkeletonLinkClusterBuffer.CheckDataIdByName(GetClusterName(meshData, skeletonUniqueName));
}

MeshSkeletonLinkClusterBase* AssetSceneData::GetMeshSkeletonLinkClusterData(int32_t clusterId)
{
	return mMeshSkeletonLinkClusterBuffer.GetData(clusterId);
}

int32_t AssetSceneData::GetAnimationInstanceMatrixDataId(const LString& animaInstanceUniqueName)
{
	return mAnimationInstanceMatrixBuffer.CheckDataIdByName(animaInstanceUniqueName);
}

AnimationInstanceMatrix* AssetSceneData::GetAnimationInstanceMatrixData(int32_t animInstanceId)
{
	return mAnimationInstanceMatrixBuffer.GetData(animInstanceId);
}

LString AssetSceneData::GetSubmeshName(SubMesh* meshData)
{
	LString primitiveName = meshData->mAssetPath + "_#_" + std::to_string(meshData->mSubmeshIndex);
	return primitiveName;
}

LString AssetSceneData::GetClusterName(SubMesh* meshData, const LString& skeletonUniqueName)
{
	return GetSubmeshName(meshData) + "_##_" + skeletonUniqueName;
}

void AssetSceneData::Update()
{
	AnimationInstanceMatrix* animationMatrixBuffer = mAnimationInstanceMatrixBuffer.GetData(0);
	MeshSkeletonLinkClusterBase* beginbuffer = mMeshSkeletonLinkClusterBuffer.GetData(0);
	LArray<LMatrix4f> skinMatrixResult;
	for(int32_t i = 0;i < beginbuffer->mBindposeMatrix.size(); ++i)
	{
		LMatrix4f skinRefMatrix = beginbuffer->mBindposeMatrix[i];
		int32_t animationBoneId = beginbuffer->mSkinBoneIndex2SkeletonBoneIndex[i];
		LMatrix4f animationMatrix = animationMatrixBuffer->mBoneMatrix[animationBoneId];
		skinMatrixResult.push_back(animationMatrix * skinRefMatrix);

	}
	sRenderModule->GetRenderContext()->UpdateConstantBuffer(mSkeletonResultBuffer, skinMatrixResult.data(), skinMatrixResult.size() * sizeof(LMatrix4f));
}

RenderScene::RenderScene()
{

}


uint64_t RenderScene::CreateRenderObject(MaterialInstance* mat, SubMesh* meshData, bool castShadow, LMatrix4f* worldMat)
{
	RenderObject* ro = new RenderObject();
	if (mRoIndex.empty())
	{
		ro->mID = mRenderObjects.size();
	}
	else
	{
		ro->mID = mRoIndex.front();
		mRoIndex.pop();
	}
	ro->mID = mRenderObjects.size();
	ro->mMeshIndex = mSceneDataGpu.AddMeshData(meshData);
	ro->mMaterial = mat;
	ro->mWorldMat = worldMat;
	ro->mCastShadow = castShadow;
	mRenderObjects.insert({ ro->mID,ro});
	UpdateRenderObject(ro->mID);
	return ro->mID;
}

uint64_t RenderScene::CreateRenderObjectDynamic(
	MaterialInstance* mat,
	SubMesh* meshData,
	const LUnorderedMap<LString, int32_t>& skeletonId,
	const LString& skeletonUniqueName,
	const LString& animaInstanceUniqueName,
	const LArray<LMatrix4f>& allBoneMatrix,
	bool castShadow,
	LMatrix4f* worldMat
)
{
	uint64_t newRoId = CreateRenderObject(mat, meshData, castShadow, worldMat);
	SetRenderObjectMeshSkletonCluster(newRoId, meshData, skeletonId, skeletonUniqueName);
	SetRenderObjectAnimInstance(newRoId, animaInstanceUniqueName, allBoneMatrix);
	return newRoId;
}

void RenderScene::UpdateRenderObject(uint64_t roId)
{
	for (int32_t passType = 0; passType < MeshRenderPass::AllNum; ++passType)
	{
		MeshRenderPass meshRenderPass = static_cast<MeshRenderPass>(passType);
		RoPassProcessorBase* roPassGen = RoPassProcessorManager::GetInstance()->GetPassProcessor(meshRenderPass)(this, meshRenderPass);
		roPassGen->AddRenderObject(mRenderObjects[roId]);
	}
}

void RenderScene::SetRenderObjectMesh(uint64_t roId, SubMesh* meshData)
{
	mRenderObjects[roId]->mMeshIndex = mSceneDataGpu.AddMeshData(meshData);
}

void RenderScene::SetRenderObjectMeshSkletonCluster(uint64_t roId, SubMesh* meshData, const LUnorderedMap<LString, int32_t>& skeletonId, const LString& skeletonUniqueName)
{
	mRenderObjects[roId]->mSkinClusterIndex = mSceneDataGpu.AddMeshSkeletonLinkClusterData(meshData, skeletonId, skeletonUniqueName);
}

void RenderScene::SetRenderObjectAnimInstance(uint64_t roId, const LString& animaInstanceUniqueName, const LArray<LMatrix4f>& allBoneMatrix)
{
	mRenderObjects[roId]->mAnimInstanceIndex = mSceneDataGpu.AddAnimationInstanceMatrixData(animaInstanceUniqueName, allBoneMatrix);
}

void RenderScene::UpdateRenderObjectAnimInstance(uint64_t roId, const LArray<LMatrix4f>& allBoneMatrix)
{
	mSceneDataGpu.UpdateAnimationInstanceMatrixData(mRenderObjects[roId]->mAnimInstanceIndex, allBoneMatrix);
}

void RenderScene::SetRenderObjectCastShadow(uint64_t roId, bool castShadow)
{
	mRenderObjects[roId]->mCastShadow = castShadow;
}

void RenderScene::SetRenderObjectTransformRef(uint64_t roId, LMatrix4f* worldMat)
{
	mRenderObjects[roId]->mWorldMat = worldMat;
}

void RenderScene::SetRenderObjectMaterial(uint64_t roId, MaterialInstance* mat)
{
	mRenderObjects[roId]->mMaterial = mat;
	UpdateRenderObject(roId);
}

void RenderScene::PrepareScene()
{

	if (!mBufferDirty)
		return;

	uint32_t shadowmapIdx = 0;
	//todo:这里dx会出现变量被优化的情况
	if (mSceneParamsBuffer == nullptr)
		mSceneParamsBuffer = new ShaderCBuffer(sRenderModule->GetRenderContext()->GetDefaultShaderConstantBufferDesc(LString("SceneBuffer").Hash()));
	if (mROIDInstancingBuffer == nullptr)
		mROIDInstancingBuffer = new ShaderCBuffer(RHIBufferUsage::VertexBufferBit, sizeof(uint32_t) * 4 * 128);

	if (mMainDirLight)
	{
		mSceneParamsBuffer->Set("cDirectionLightColor", LMath::sRGB2LinearColor(mMainDirLight->mColor));
		mSceneParamsBuffer->Set("cLightDirection", mMainDirLight->mDirection);
	}
	else
	{
		mSceneParamsBuffer->Set("cDirectionLightColor", LVector4f(0,0,0,0));
	}	
	
	mSceneParamsBuffer->Set("cPointLightsCount", mPointLights.size());
	mSceneParamsBuffer->Set("cShadowmapCount", 0);
	for (int i = 0; i < mPointLights.size(); i++)
	{
		PointLight* light = mPointLights[i];
		if(light->mCastShadow)
			mSceneParamsBuffer->Set("cShadowmapCount", 6);  

		mSceneParamsBuffer->Set("cPointLights", light->mPosition, i, 0);
		mSceneParamsBuffer->Set("cPointLights", LMath::sRGB2LinearColor(light->mColor), i, 16);
		mSceneParamsBuffer->Set("cPointLights", light->mIntensity, i, 32);
	}
	mSceneParamsBuffer->Set("cAmbientColor", LMath::sRGB2LinearColor(mAmbientColor));
	for (auto& ro : mRenderObjects)
	{		
		uint32_t idx = ro.second->mID;
		mSceneParamsBuffer->Set("cRoWorldMatrix", * ro.second->mWorldMat, idx);
		mROIDInstancingBuffer->Set(idx * sizeof(uint32_t) * 4, idx);
	}
	
	
}

DirectionLight* RenderScene::CreateMainDirLight()
{
	mMainDirLight = new DirectionLight();
	mMainDirLight->mOwnerScene = this;
	return mMainDirLight;
}

PointLight* RenderScene::CreatePointLight()
{
	auto light = new PointLight();
	light->mOwnerScene = this;
	mPointLights.push_back(light);
	return light;
}

RenderView* RenderScene::CreateRenderView()
{
	RenderView* newView = new RenderView();
	newView->mOwnerScene = this;
	mViews.push_back(newView);
	return newView;
}

void RenderScene::Render(FrameGraphBuilder* FG)
{
	mSceneDataGpu.Update();
	for (auto data : mDatas)
	{
		data->PerSceneUpdate(this);
	}

	PrepareScene();

	for (RenderView* renderView : mViews)
	{
		renderView->PrepareView();	
	}

	Debug();

	mSceneParamsBuffer->Commit();
	mROIDInstancingBuffer->Commit();

	for (RenderView* renderView : mViews)
	{
		renderView->Render(this, FG);
	}
}

void RenderScene::DestroyMainDirLight(DirectionLight* val)
{
	if (mMainDirLight == val)
	{
		delete mMainDirLight;
		mMainDirLight = nullptr;
	}
}

void RenderScene::DestroyRenderView(RenderView* renderView)
{
	for (auto it = mViews.begin(); it != mViews.end(); ++it)
	{
		if (renderView == *it)
		{			
			mViews.erase(it);			
			delete renderView;
			return;
		}
	}
}

void RenderScene::DestroyLight(Light* ro)
{
	for (auto it = mPointLights.begin(); it != mPointLights.end(); ++it)
	{
		if (ro == *it)
		{
			mPointLights.erase(it);
			delete ro;
			break;
		}
	}	
	mBufferDirty = true;
}

void RenderScene::DestroyRenderObject(uint64_t ro)
{
	auto check = mRenderObjects.find(ro);
	if (check == mRenderObjects.end())
	{
		mRoIndex.push(check->first);
		delete check->second;
		mRenderObjects.erase(check);
		
	}
	mBufferDirty = true;
}

RenderScene::~RenderScene()
{
	for (RenderView* it : mViews)
	{
		delete it;
	}
	if (mROIDInstancingBuffer)
	{
		delete mROIDInstancingBuffer;
		mROIDInstancingBuffer = nullptr;
	}
	for (PointLight* it : mPointLights)
	{
		delete it;
	}
	if (mSceneParamsBuffer)
	{
		delete mSceneParamsBuffer;
		mSceneParamsBuffer = nullptr;
	}
	mViews.clear();
	for (auto it : mRenderObjects)
	{
		delete it.second;
	}
	mRenderObjects.clear();
}

void RenderScene::Debug()
{
	if (!mDrawGizmos)
		return;

	SubMesh            mDebugMeshLine;
	SubMesh            mDebugMesh;
	if (mMainDirLight && mMainDirLight->mCastShadow)
	{
		LFrustum f = LFrustum::FromOrth(0.01, 50, 30, 30);
		f.Multiple(mMainDirLight->mViewMatrix.inverse());
		AddLineToSubMesh(f.mNearPlane[0], f.mNearPlane[1], mDebugMeshLine);
		AddLineToSubMesh(f.mNearPlane[1], f.mNearPlane[2], mDebugMeshLine);
		AddLineToSubMesh(f.mNearPlane[2], f.mNearPlane[3], mDebugMeshLine);
		AddLineToSubMesh(f.mNearPlane[3], f.mNearPlane[0], mDebugMeshLine);
		AddLineToSubMesh(f.mNearPlane[0], f.mFarPlane[0], mDebugMeshLine);
		AddLineToSubMesh(f.mNearPlane[1], f.mFarPlane[1], mDebugMeshLine);
		AddLineToSubMesh(f.mNearPlane[2], f.mFarPlane[2], mDebugMeshLine);
		AddLineToSubMesh(f.mNearPlane[3], f.mFarPlane[3], mDebugMeshLine);
		AddLineToSubMesh(f.mFarPlane[0], f.mFarPlane[1], mDebugMeshLine);
		AddLineToSubMesh(f.mFarPlane[1], f.mFarPlane[2], mDebugMeshLine);
		AddLineToSubMesh(f.mFarPlane[2], f.mFarPlane[3], mDebugMeshLine);
		AddLineToSubMesh(f.mFarPlane[3], f.mFarPlane[0], mDebugMeshLine);
	}

	for (int i = 0; i < mPointLights.size(); i++)
	{
		PointLight* light = mPointLights[i];
		if (light->mCastShadow)
		{
			for (int faceIdx = 0; faceIdx < 6; faceIdx++)
			{
				LFrustum f = LFrustum::MakeFrustrum(light->mFov, light->mNear, light->mFar, light->mAspect);
				f.Multiple(light->mViewMatrix[faceIdx].inverse());

				AddLineToSubMesh(f.mNearPlane[0], f.mNearPlane[1], mDebugMeshLine);
				AddLineToSubMesh(f.mNearPlane[1], f.mNearPlane[2], mDebugMeshLine);
				AddLineToSubMesh(f.mNearPlane[2], f.mNearPlane[3], mDebugMeshLine);
				AddLineToSubMesh(f.mNearPlane[3], f.mNearPlane[0], mDebugMeshLine);
				AddLineToSubMesh(f.mNearPlane[0], f.mFarPlane[0], mDebugMeshLine);
				AddLineToSubMesh(f.mNearPlane[1], f.mFarPlane[1], mDebugMeshLine);
				AddLineToSubMesh(f.mNearPlane[2], f.mFarPlane[2], mDebugMeshLine);
				AddLineToSubMesh(f.mNearPlane[3], f.mFarPlane[3], mDebugMeshLine);
				AddLineToSubMesh(f.mFarPlane[0], f.mFarPlane[1], mDebugMeshLine);
				AddLineToSubMesh(f.mFarPlane[1], f.mFarPlane[2], mDebugMeshLine);
				AddLineToSubMesh(f.mFarPlane[2], f.mFarPlane[3], mDebugMeshLine);
				AddLineToSubMesh(f.mFarPlane[3], f.mFarPlane[0], mDebugMeshLine);
			}
			
		}
		AddCubeWiredToSubMesh(mDebugMeshLine,light->mPosition, LVector3f(1, 1, 1), light->mColor);
	}

	mDebugMeshData.Init(&mDebugMesh);
	mDebugMeshLineData.Init(&mDebugMeshLine);
}

RoPassProcessorBase::RoPassProcessorBase(RenderScene* scene, MeshRenderPass passType) : mScene(scene), mPassType(passType)
{

}

void RoPassProcessorBase::BuildMeshRenderCommands(
	RenderObject* renderObject,
	MaterialInstance* materialInstance
)
{
	size_t newCommandId = mScene->mAllMeshDrawCommands[mPassType].AddCommand(renderObject, materialInstance);
	renderObject->mAllCommandsIndex.insert({ mPassType,newCommandId });
}

RoPassProcessorManager* RoPassProcessorManager::mInstance = nullptr;

RoPassProcessorManager::RoPassProcessorManager()
{

}

RoPassProcessorManager* RoPassProcessorManager::GetInstance()
{
	if (mInstance == nullptr)
	{
		mInstance = new RoPassProcessorManager();
		mInstance->Init();
	}
	return mInstance;
}

void RoPassProcessorManager::RegistorPassProcessor(MeshRenderPass passType, MeshPassProcessorCreateFunction passProcessorGenerator)
{
	mAllPassProcessor.insert({ passType ,passProcessorGenerator });
}

MeshPassProcessorCreateFunction RoPassProcessorManager::GetPassProcessor(MeshRenderPass passType)
{
	return mAllPassProcessor[passType];
}

}

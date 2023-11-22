#include "Game/MeshRenderer.h"
#include "Core/Object/Transform.h"
#include "Game/Scene.h"

#include "Core/Memory/PtrBinding.h"
#include "Animation/AnimationModule.h"
#include "Core/Asset/AssetModule.h"
namespace luna
{
namespace graphics
{

void GameStaticMeshRenderDataUpdater::UpdateRenderThreadImpl(graphics::GameRenderBridgeData* curData, graphics::RenderScene* curScene) 
{
	GameRenderBridgeDataStaticMesh* realPointer = static_cast<GameRenderBridgeDataStaticMesh*>(curData);
	if (realPointer->mNeedIniteMesh)
	{
		assert(mRenderMesh.size() == 0);
		for (int32_t subMeshIndex = 0; subMeshIndex < realPointer->mInitSubmesh.size(); ++subMeshIndex)
		{
			graphics::RenderObject* newObject = curScene->CreateRenderObject();
			RenderMeshBase* meshData = newObject->RequireData<RenderMeshBase>();
			meshData->mMeshData = realPointer->mInitSubmesh[subMeshIndex]->GetRenderMeshData();
			mRenderMesh.push_back(meshData);
			mRenderObjects.push_back(newObject);
		}
	}
	if (realPointer->mNeedIniteMaterial)
	{
		for (int32_t refreshId = 0; refreshId < realPointer->mMaterialInitIndex.size(); ++refreshId)
		{
			size_t roIndex = realPointer->mMaterialInitIndex[refreshId];
			mRenderMesh[roIndex]->mMaterial = realPointer->mInitMaterial[roIndex]->CreateInstance();
			mRenderMesh[roIndex]->mMaterial->Ready();
		}
	}
	if (realPointer->NeedUpdateStaticMessage)
	{
		for (int32_t subMeshIndex = 0; subMeshIndex < mRenderMesh.size(); ++subMeshIndex)
		{
			mRenderMesh[subMeshIndex]->mCastShadow = realPointer->mCastShadow;
			mRenderMesh[subMeshIndex]->mReceiveLight = realPointer->mReceiveLight;
			mRenderMesh[subMeshIndex]->mReceiveShadow = realPointer->mReceiveShadow;
		}
	}
}

void GameStaticMeshRenderDataUpdater::ClearData(graphics::GameRenderBridgeData* curData)
{
	GameRenderBridgeDataStaticMesh* realPointer = static_cast<GameRenderBridgeDataStaticMesh*>(curData);
	if (realPointer->mNeedIniteMesh)
	{
		realPointer->mInitSubmesh.clear();
	}
	realPointer->mNeedIniteMesh = false;
	if (realPointer->mNeedIniteMaterial)
	{
		realPointer->mMaterialInitIndex.clear();
		realPointer->mInitMaterial.clear();
	}
	realPointer->mNeedIniteMaterial = false;
	realPointer->NeedUpdateStaticMessage = false;
}

void GameSkeletalMeshRenderDataUpdater::UpdateRenderThreadImpl(graphics::GameRenderBridgeData* curData, graphics::RenderScene* curScene)
{
	GameStaticMeshRenderDataUpdater::UpdateRenderThreadImpl(curData, curScene);
	GameRenderBridgeDataSkeletalMesh* realPointer = static_cast<GameRenderBridgeDataSkeletalMesh*>(curData);
	if (realPointer->mNeedInitSkinMessage)
	{
		for (int32_t subMeshIndex = 0; subMeshIndex < mRenderObjects.size(); ++subMeshIndex)
		{
			SkeletonSkinData* skinData = mRenderObjects[subMeshIndex]->RequireData<SkeletonSkinData>();
			SubMeshSkeletal* skeletalMeshPointer = static_cast<SubMeshSkeletal*>(realPointer->mInitSubmesh[subMeshIndex]);
			skinData->Create(realPointer->mSkeletonUniqueName, skeletalMeshPointer, realPointer->mBindCluster[subMeshIndex].mSkeletonId);
			mSkinData.push_back(skinData);
		}
	}
}

void GameSkeletalMeshRenderDataUpdater::ClearData(graphics::GameRenderBridgeData* curData)
{
	GameStaticMeshRenderDataUpdater::ClearData(curData);
	GameRenderBridgeDataSkeletalMesh* realPointer = static_cast<GameRenderBridgeDataSkeletalMesh*>(curData);
	if (realPointer->mNeedInitSkinMessage)
	{
		realPointer->mBindCluster.clear();
	}
	realPointer->mNeedInitSkinMessage = false;
};



RegisterTypeEmbedd_Imp(StaticMeshRenderer)
{
	cls->Ctor<StaticMeshRenderer>();
	cls->Binding<Self>();

	cls->BindingProperty<&Self::mMaterialAsset>("material")
		.Serialize();

	cls->BindingProperty<&StaticMeshRenderer::mCastShadow>("cast_shadow")
		.Setter<&StaticMeshRenderer::SetCastShadow>()
		.Serialize();

	cls->BindingProperty<&Self::mMeshAsset>("mesh")
		.Setter<&StaticMeshRenderer::SetMeshAsset>()
		.Serialize();

	BindingModule::Luna()->AddType(cls);
}

void StaticMeshRenderer::OnCreate()
{
	Super::OnCreate();
	mMeshDirty = true;
	mMaterialDirty = true;

}

void StaticMeshRenderer::OnActivate()
{
}

void StaticMeshRenderer::SetMeshAsset(MeshAsset* obj)
{
	mMeshDirty = true;
}

StaticMeshRenderer::~StaticMeshRenderer()
{

}

void StaticMeshRenderer::SetMaterialAsset(int32_t matIndex, const LString& assetPath)
{
	mMaterialDirty = true;
	luna::LType* createType = LType::Get<MaterialTemplateAsset>();
	mMaterialAsset.GetPtr(matIndex) = (MaterialTemplateAsset*)sAssetModule->NewAsset(assetPath, createType);
	mAllDirtyMaterial.push_back(matIndex);
}



void StaticMeshRenderer::SetCastShadow(bool val)
{
	mRenderParamDirty = true;
	mCastShadow = val;
}

RegisterTypeEmbedd_Imp(SkeletonMeshRenderer)
{
	cls->Ctor<SkeletonMeshRenderer>();
	cls->Binding<Self>();

	cls->BindingProperty<&Self::mSkeletonAsset>("skeleton")
		.Setter<&SkeletonMeshRenderer::SetSkeletonAsset>()
		.Serialize();

	cls->BindingProperty<&Self::mSkelAnimAsset>("skelanim")
		.Setter<&SkeletonMeshRenderer::SetSkelAnimationAsset>()
		.Serialize();
	BindingModule::Luna()->AddType(cls);
}

void SkeletonMeshRenderer::GetSkeletonPoseMatrix(LArray<LMatrix4f>& poseMatrix)
{
	for (int32_t i = 0; i < mSkeletonAsset->GetBoneCount(); ++i)
	{
		const animation::LSingleBone& boneData = mSkeletonAsset->GetBone(i);
		LMatrix4f worldSpacePose = LMath::MatrixCompose(boneData.mBaseTranslation, boneData.mBaseRotation, boneData.mBaseScal);
		if (boneData.mParentIndex != uint32_t(-1))
		{
			worldSpacePose = poseMatrix[boneData.mParentIndex] * worldSpacePose;
		}
		poseMatrix.push_back(worldSpacePose);
	}
}

void SkeletonMeshRenderer::SetSkeletonAsset(animation::SkeletonAsset* obj)
{
	mSkinDirty = true;
	mSkeletonAsset = ToSharedPtr(obj);

	if (mSkelAnimAsset != nullptr)
	{
		animation::AnimationModule* animModule = gEngine->GetTModule<animation::AnimationModule>();
		if (mAnimationInstance != nullptr)
		{
			animModule->FreeAnimationInstance(mAnimationInstance->GetAnimIndex());
		}
		mAnimationInstance = animModule->CreateAnimationInstanceClip(mSkelAnimAsset.get(), mSkeletonAsset.get());
	}
}

void SkeletonMeshRenderer::SetSkelAnimationAsset(animation::AnimationClipAsset* obj)
{
	mSkelAnimAsset = ToSharedPtr(obj);
	if (mSkeletonAsset != nullptr)
	{
		animation::AnimationModule* animModule = gEngine->GetTModule<animation::AnimationModule>();
		mAnimationInstance = animModule->CreateAnimationInstanceClip(mSkelAnimAsset.get(), mSkeletonAsset.get());
	}
}

}
}
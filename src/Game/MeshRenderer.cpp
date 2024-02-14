#include "Game/MeshRenderer.h"
#include "Core/Object/Transform.h"
#include "Game/Scene.h"

#include "Core/Memory/PtrBinding.h"
#include "Animation/AnimationModule.h"
#include "Core/Asset/AssetModule.h"
#include "Graphics/Renderer/SkeletonSkin.h"
namespace luna
{
namespace graphics
{

RegisterTypeEmbedd_Imp(StaticMeshRenderer)
{
	cls->Ctor<StaticMeshRenderer>();
	cls->Binding<Self>();

	cls->BindingProperty<&Self::mMaterialAsset>("material")
		.Serialize();

	cls->BindingProperty<&StaticMeshRenderer::mCastShadow>("cast_shadow")
		.Setter<&StaticMeshRenderer::SetCastShadow>()
		.Serialize();

	cls->BindingProperty<&Self::mMeshAssetPath>("mesh")
		.Setter<&StaticMeshRenderer::SetMeshAsset>()
		.Serialize();

	BindingModule::Luna()->AddType(cls);
}

void StaticMeshRenderer::OnTransformDirty(Transform* transform)
{
	graphics::MeshRendererUpdateReceiveLightCommand(
		GetScene()->GetRenderScene(),
		mVirtualRenderData, mTransform->GetLocalToWorldMatrix());
}

void StaticMeshRenderer::OnCreate()
{
	Super::OnCreate();
	mTransformDirtyAction = mOwnerEntity->GetComponent<Transform>()->OnTransformDirty.Bind(AutoBind(&StaticMeshRenderer::OnTransformDirty, this));
	OnTransformDirty(mOwnerEntity->GetComponent<Transform>());
}

void StaticMeshRenderer::OnActivate()
{
}

void StaticMeshRenderer::GenerateMeshAsset(const LString& path)
{
	mMeshAsset = sAssetModule->LoadAsset<MeshAsset>(path);
}

void StaticMeshRenderer::SetMeshAsset(LString meshAssetPath)
{
	GenerateMeshAsset(meshAssetPath);
	if (GetScene())
	{
		graphics::MeshRendererDataGenerateCommand(
			GetScene()->GetRenderScene(),
			mVirtualRenderData, mMeshAsset->mSubMesh);
		for (int32_t i = 0; i < mMeshAsset->mSubMesh.size(); ++i)
		{
			graphics::MeshRendererMaterialGenerateCommand(
				GetScene()->GetRenderScene(),
				mVirtualRenderData, i, mMaterialAsset.get());
		}
		OnMeshAssetChange();
	}
}

void StaticMeshRenderer::OnMeshAssetChange()
{

}

StaticMeshRenderer::~StaticMeshRenderer()
{

}

void StaticMeshRenderer::SetMaterialAsset(int32_t matIndex, const LString& assetPath)
{
	graphics::MeshRendererMaterialGenerateCommand(
		GetScene()->GetRenderScene(),
		mVirtualRenderData, matIndex, mMaterialAsset.get());
}


void StaticMeshRenderer::SetCastShadow(bool val)
{
	mCastShadow = val;
	if (GetScene())
	{
		graphics::MeshRendererUpdateCastShadowCommand(GetScene()->GetRenderScene(), mVirtualRenderData, mCastShadow);
	}
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
	mSkeletonAsset = ToSharedPtr(obj);

	if (mSkelAnimAsset != nullptr)
	{
		animation::AnimationModule* animModule = gEngine->GetTModule<animation::AnimationModule>();
		if (mAnimationInstance != nullptr)
		{
			animModule->FreeAnimationInstance(mAnimationInstance->GetAnimIndex());
		}
		mAnimationInstance = animModule->CreateAnimationInstanceClip(mSkelAnimAsset.get(), mSkeletonAsset.get());
		if (mMeshAsset != nullptr)
		{
			UpdateSkinData();
		}
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

void SkeletonMeshRenderer::OnMeshAssetChange()
{
	if (mSkeletonAsset)
	{
		UpdateSkinData();
	}
};

void SkeletonMeshRenderer::GenerateMeshAsset(const LString& path)
{
	mMeshAsset = sAssetModule->LoadAsset<SkeletalMeshAsset>(path);
}

void SkeletonMeshRenderer::UpdateSkinData()
{
	LString skeletonUniqueName = mSkeletonAsset->GetAssetPath();
	LArray<LMatrix4f> allBoneMatrix;
	LArray<SkeletonBindPoseMatrix> bindCluster;
	bindCluster.resize(mMeshAsset->mSubMesh.size());
	for (int32_t subIndex = 0; subIndex < mMeshAsset->mSubMesh.size(); ++subIndex)
	{
		bindCluster[subIndex].mSkeletonId = mSkeletonAsset->GetSearchIndex();
	}
	graphics::MeshRendererUpdateSkinCommand(
		GetScene()->GetRenderScene(),
		mVirtualRenderData,
		mMeshAsset->mSubMesh,
		skeletonUniqueName, bindCluster
	);
}

}
}
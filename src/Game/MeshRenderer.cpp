#include "Game/MeshRenderer.h"
#include "Core/Object/Transform.h"
#include "Game/Scene.h"

#include "Core/Memory/PtrBinding.h"

namespace luna
{
namespace render
{

RegisterTypeEmbedd_Imp(MeshRenderer)
{
	cls->Ctor<MeshRenderer>();
	cls->Binding<Self>();

	cls->BindingProperty<&Self::mMaterialAsset>("material")
		.Setter<&MeshRenderer::SetMaterial>()
		.Serialize();

	cls->BindingProperty<&MeshRenderer::mCastShadow>("cast_shadow")
		.Setter<&MeshRenderer::SetCastShadow>()
		.Serialize();

	BindingModule::Luna()->AddType(cls);
}

void MeshRenderer::OnCreate()
{
	Super::OnCreate();
	if (mMaterialAsset)
	{
		mMaterialInstance = mMaterialAsset->GetDefaultInstance();
		mMaterialInstance->Ready();
	}
	CreateRenderObject();
}

void MeshRenderer::OnActivate()
{
}

MeshRenderer::~MeshRenderer()
{
	if (mRO && GetScene() && GetScene()->GetRenderScene())
		GetScene()->GetRenderScene()->DestroyRenderObject(mRO);
}

void MeshRenderer::SetMaterial(MaterialTemplateAsset* mat)
{
	mMaterialAsset = ToSharedPtr(mat);
	mMaterialInstance = mat->CreateInstance();
	if (mMaterialInstance)
		mMaterialInstance->Ready();
	if (mRO == uint64_t(-1))
	{
		return;
	}
	GetScene()->GetRenderScene()->SetRenderObjectMaterial(mRO, mMaterialInstance.Get());
}



void MeshRenderer::SetCastShadow(bool val)
{
	if (mRO == uint64_t(-1))
	{
		return;
	}
	mCastShadow = val;
	GetScene()->GetRenderScene()->SetRenderObjectCastShadow(mRO, mCastShadow);
}

RegisterTypeEmbedd_Imp(StaticMeshRenderer)
{
	cls->Ctor<StaticMeshRenderer>();
	cls->Binding<Self>();

	cls->BindingProperty<&Self::mMeshAsset>("mesh")
		.Setter<&StaticMeshRenderer::SetMeshAsset>()
		.Serialize();

	BindingModule::Luna()->AddType(cls);
}

void StaticMeshRenderer::SetMeshAsset(MeshAsset* obj)
{
	mMeshAsset = ToSharedPtr(obj);
	if (mRO == uint64_t(-1))
	{
		return;
	}
	GetScene()->GetRenderScene()->SetRenderObjectMesh(mRO, mMeshAsset->GetSubMeshAt(0));
}

void StaticMeshRenderer::CreateRenderObject()
{
	if (mRO != uint64_t(-1) || !GetScene())
	{
		return;
	}
	mRO = GetScene()->GetRenderScene()->CreateRenderObject(
		mMaterialInstance.Get(),
		GetMeshAsset()->GetSubMeshAt(0),
		mCastShadow,
		&mTransform->GetLocalToWorldMatrix()
	);

}

RegisterTypeEmbedd_Imp(SkeletonMeshRenderer)
{
	cls->Ctor<SkeletonMeshRenderer>();
	cls->Binding<Self>();

	cls->BindingProperty<&Self::mSkeletalMeshAsset>("mesh")
		.Setter<&SkeletonMeshRenderer::SetMeshAsset>()
		.Serialize();

	cls->BindingProperty<&Self::mSkeletonAsset>("skeleton")
		.Setter<&SkeletonMeshRenderer::SetSkeletonAsset>()
		.Serialize();

	BindingModule::Luna()->AddType(cls);
}

void SkeletonMeshRenderer::SetMeshAsset(SkeletalMeshAsset* obj)
{
	mSkeletalMeshAsset = ToSharedPtr(obj);
	if (mRO == uint64_t(-1))
	{
		return;
	}
	GetScene()->GetRenderScene()->SetRenderObjectMesh(mRO, mSkeletalMeshAsset->GetSubMeshAt(0));
}

void SkeletonMeshRenderer::GetSkeletonPoseMatrix(LArray<LMatrix4f>& poseMatrix)
{
	for (int32_t i = 0; i < mSkeletonAsset->GetBoneCount(); ++i)
	{
		const animation::LSingleBone& boneData = mSkeletonAsset->GetBone(i);
		poseMatrix.push_back(LMath::MatrixCompose(boneData.mBaseTranslation, boneData.mBaseRotation, boneData.mBaseScal));
	}
}

void SkeletonMeshRenderer::SetSkeletonAsset(animation::SkeletonAsset* obj)
{
	mSkeletonAsset = ToSharedPtr(obj);
	if (mRO == uint64_t(-1))
	{
		return;
	}
	LString skeletonUniqueName = mSkeletonAsset->GetAssetPath();
	GetScene()->GetRenderScene()->SetRenderObjectMeshSkletonCluster(mRO, mSkeletalMeshAsset->GetSubMeshAt(0), mSkeletonAsset->GetSearchIndex(), skeletonUniqueName);
	LArray<LMatrix4f> allBoneMatrix;
	GetSkeletonPoseMatrix(allBoneMatrix);
	GetScene()->GetRenderScene()->SetRenderObjectAnimInstance(mRO, skeletonUniqueName, allBoneMatrix);
}

void SkeletonMeshRenderer::CreateRenderObject()
{
	if (mRO != uint64_t(-1) || !GetScene())
	{
		return;
	}
	LString skeletonUniqueName = mSkeletonAsset->GetAssetPath();
	LArray<LMatrix4f> allBoneMatrix;
	GetSkeletonPoseMatrix(allBoneMatrix);
	mRO = GetScene()->GetRenderScene()->CreateRenderObjectDynamic(
		mMaterialInstance.Get(),
		GetMeshAsset()->GetSubMeshAt(0),
		mSkeletonAsset->GetSearchIndex(),
		skeletonUniqueName,
		skeletonUniqueName,
		allBoneMatrix,
		mCastShadow, 
		&mTransform->GetLocalToWorldMatrix()
	);

}

}
}
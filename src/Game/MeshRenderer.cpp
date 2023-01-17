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

	cls->BindingProperty<&Self::mObjAsset>("mesh")
		.Setter<&MeshRenderer::SetObjAsset>()
		.Serialize();

	cls->BindingProperty<&Self::mMaterialAsset>("material")
		.Setter<&MeshRenderer::SetMaterial>()
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
	if (mRO)
		GetScene()->GetRenderScene()->DestroyRenderObject(mRO);
}

void MeshRenderer::CreateRenderObject()
{	
	if (mRO)
		return;

	if (mMaterialInstance && mObjAsset)
	{
		mRO = GetScene()->GetRenderScene()->CreateRenderObject();
		mRO->mMaterial = mMaterialInstance.Get();
		mRO->mMesh = mObjAsset->GetSubMeshAt(0);
		if (mRO->mMesh->mVB.get() == nullptr)
		{
			mRO->mMesh->Init();
		}
		mRO->mWorldMat = &(mTransform->GetLocalToWorldMatrix());
	}
	
}

void MeshRenderer::SetMaterial(MaterialTemplateAsset* mat)
{
	mMaterialAsset = mat;
	mMaterialInstance = mat->CreateInstance();
	if (mMaterialInstance)
		mMaterialInstance->Ready();
	if (!mRO)
	{
		CreateRenderObject();
	}
	else
	{
		mRO->mMaterial = mMaterialInstance.Get();
	}
}

void MeshRenderer::SetObjAsset(ObjAsset* obj)
{
	mObjAsset = obj;
	if (!mRO)
	{
		CreateRenderObject();
	}
	else
	{
		mRO->mMesh = obj->GetSubMeshAt(0);
	}
}

}
}
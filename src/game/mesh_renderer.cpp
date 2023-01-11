#include "mesh_renderer.h"
#include "core/object/transform.h"
#include "game/scene.h"


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

	cls->BindingProperty<&Self::mMaterialInstance>("material")
		.Serialize();

	BindingModule::Luna()->AddType(cls);
}

void MeshRenderer::OnCreate()
{
	if (!mMaterialInstance)
	{
		mMaterialInstance = NewObject<MaterialInstance>();
	}
	mMaterialInstance->SetParent(this);
	mMaterialInstance->Ready();
	Super::OnCreate();
	mRO = GetScene()->GetRenderScene()->CreateRenderObject();
	mRO->mMaterial = mMaterialInstance.Get();
	mRO->mMesh = mObjAsset->GetSubMeshAt(0);
	if (mRO->mMesh->mVB.get() == nullptr)
	{
		mRO->mMesh->Init();
	}
	mRO->mWorldMat = &(mTransform->GetLocalToWorldMatrix());
}

void MeshRenderer::OnActivate()
{
}

MeshRenderer::~MeshRenderer()
{
	if (mRO)
		GetScene()->GetRenderScene()->DestroyRenderObject(mRO);
}

}
}
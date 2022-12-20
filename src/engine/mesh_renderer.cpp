#include "mesh_renderer.h"
#include "core/object/transform.h"
#include "engine/scene.h"

namespace luna
{
namespace render
{

RegisterTypeEmbedd_Imp(MeshRenderer)
{
	cls->Ctor<MeshRenderer>();
	cls->Binding<Self>();
	cls->BindingProperty<&Self::mObjAsset>("mesh");
	cls->BindingProperty<&Self::mMaterialInstance>("material");
	BindingModule::Luna()->AddType(cls);
}

bool MeshRenderer::PopulateRenderNode(RenderObject& render_nodes)
{
	if (m_mesh && mMaterialInstance)
	{
		render_nodes.mMaterial = mMaterialInstance.Get();
		render_nodes.mMesh = m_mesh->GetSubMeshAt(0);
		if (render_nodes.mMesh->mVB.get() == nullptr)
		{
			render_nodes.mMesh->Init();
		}
		render_nodes.mWorldMat = &(mTransform->GetLocalToWorldMatrix());
		render_nodes.mCastShadow = GetCastShadow();
		return true;
	}
	return false;
}

void MeshRenderer::OnCreate()
{
	if (!mMaterialInstance)
		mMaterialInstance = NewObject<MaterialInstance>();
	mMaterialInstance->Ready();
	Super::OnCreate();
	auto ro  = GetScene()->GetRenderScene()->CreateRenderObject();
	ro->mMaterial = mMaterialInstance.Get();
	ro->mMesh = mObjAsset->GetSubMeshAt(0);
	if (ro->mMesh->mVB.get() == nullptr)
	{
		ro->mMesh->Init();
	}
	ro->mWorldMat = &(mTransform->GetLocalToWorldMatrix());
}

void MeshRenderer::OnActivate()
{
}

}
}
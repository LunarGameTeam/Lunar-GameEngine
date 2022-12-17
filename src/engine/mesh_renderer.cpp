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
	cls->Property< &Self::mObjAsset>("mesh");
	cls->Property< &Self::m_material>("material");
}

bool MeshRenderer::PopulateRenderNode(RenderObject& render_nodes)
{
	if (m_mesh && m_material)
	{
		render_nodes.mMaterial = m_material.Get();
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
	if (!m_material)
		m_material = NewObject<MaterialInstance>();
	m_material->Ready();
	Super::OnCreate();
	auto ro  = GetScene()->GetRenderScene()->CreateRenderObject();
	ro->mMaterial = m_material.Get();
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
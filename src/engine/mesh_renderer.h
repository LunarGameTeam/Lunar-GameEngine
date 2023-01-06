#pragma once

#include "render/render_config.h"
#include "render/asset/mesh_asset.h"
#include "render/asset/texture_asset.h"

#include "engine/render_component.h"
#include "render/asset/obj_asset.h"


namespace luna::render
{

class ENGINE_API MeshRenderer : public RendererComponent
{
	RegisterTypeEmbedd(MeshRenderer, RendererComponent)
public:
	MeshRenderer() :
		m_mesh(this),
		mMaterialInstance(this),
		mObjAsset(this)
	{

	}

	bool PopulateRenderNode(RenderObject& render_nodes) override;

	void SetMesh(MeshAsset* mesh)
	{
		m_mesh = mesh;
		m_dirty = true;
	}

	void SetMaterial(MaterialInstance* mat)
	{
		mat->SetParent(this);
		mMaterialInstance.SetPtr(mat);
		m_dirty = true;
	}

	void OnCreate() override;


	void OnActivate() override;

private:
	TPPtr<ObjAsset> mObjAsset;
	TPPtr<MeshAsset> m_mesh;
	TPPtr<MaterialInstance> mMaterialInstance;
};


class RENDER_API StaticMeshRenderer : public MeshRenderer
{
};


class RENDER_API SkeletonMeshRenderer : public MeshRenderer
{
};

}

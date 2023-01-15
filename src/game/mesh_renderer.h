#pragma once

#include "core/core_min.h"

#include "game/render_component.h"

#include "render/render_config.h"
#include "render/renderer/material.h"

#include "render/asset/mesh_asset.h"
#include "render/asset/texture_asset.h"

#include "render/asset/obj_asset.h"
#include "render/asset/material_template.h"


namespace luna::render
{

class GAME_API MeshRenderer : public RendererComponent
{
	RegisterTypeEmbedd(MeshRenderer, RendererComponent)
public:
	MeshRenderer() :
		m_mesh(this),
		mMaterialInstance(this),
		mObjAsset(this),
		mMaterialAsset(this)
	{

	}

	virtual ~MeshRenderer();

	void SetMesh(MeshAsset* mesh)
	{
		m_mesh = mesh;
		m_dirty = true;
	}
	void SetObjAsset(ObjAsset* obj);
	void SetMaterial(MaterialTemplateAsset* mat);

	void OnCreate() override;


	void OnActivate() override;
protected:
	void CreateRenderObject();
private:
	render::RenderObject* mRO = nullptr;
	TPPtr<ObjAsset> mObjAsset;
	TPPtr<MeshAsset> m_mesh;
	TPPtr<render::MaterialTemplateAsset> mMaterialAsset;
	TPPtr<render::MaterialInstance> mMaterialInstance;
};


class RENDER_API StaticMeshRenderer : public MeshRenderer
{
};


class RENDER_API SkeletonMeshRenderer : public MeshRenderer
{
};

}

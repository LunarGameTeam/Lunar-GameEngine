#pragma once

#include "game/render_component.h"

#include "render/render_config.h"
#include "render/renderer/material.h"

#include "render/asset/mesh_asset.h"
#include "render/asset/texture_asset.h"

#include "render/asset/obj_asset.h"


namespace luna::render
{

class GAME_API MeshRenderer : public RendererComponent
{
	RegisterTypeEmbedd(MeshRenderer, RendererComponent)
public:
	MeshRenderer() :
		m_mesh(this),
		mMaterialInstance(this),
		mObjAsset(this)
	{

	}

	virtual ~MeshRenderer();

	void SetMesh(MeshAsset* mesh)
	{
		m_mesh = mesh;
		m_dirty = true;
	}
	void SetObjAsset(ObjAsset* obj)
	{
		if (mRO)
			mRO->mMesh = obj->GetSubMeshAt(0);
		mObjAsset = obj;
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
	render::RenderObject* mRO;
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

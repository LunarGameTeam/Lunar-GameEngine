#pragma once

#include "Core/CoreMin.h"

#include "game/RenderComponent.h"

#include "Graphics/RenderConfig.h"
#include "Graphics/Renderer/MaterialInstance.h"

#include "Graphics/Asset/MeshAsset.h"
#include "Graphics/Asset/TextureAsset.h"

#include "Graphics/Asset/ObjAsset.h"
#include "Graphics/Asset/MaterialTemplate.h"


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

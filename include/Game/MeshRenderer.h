#pragma once

#include "Core/CoreMin.h"

#include "Game/RenderComponent.h"

#include "Graphics/RenderConfig.h"
#include "Graphics/Renderer/MaterialInstance.h"

#include "Graphics/Asset/MeshAsset.h"
#include "Graphics/Asset/TextureAsset.h"

#include "Graphics/Asset/MaterialTemplate.h"


namespace luna::render
{

class GAME_API MeshRenderer : public RendererComponent
{
	RegisterTypeEmbedd(MeshRenderer, RendererComponent)
public:
	MeshRenderer() :
		mMeshAsset(this),
		mMaterialInstance(this),
		mMaterialAsset(this)
	{

	}

	virtual ~MeshRenderer();

	void SetMeshAsset(MeshAsset* mesh);

	void SetMaterial(MaterialTemplateAsset* mat);
	void SetCastShadow(bool val);
	void OnCreate() override;


	void OnActivate() override;
protected:
	void CreateRenderObject();
private:
	bool mCastShadow = false;
	render::RenderObject* mRO = nullptr;
	TPPtr<MeshAsset> mMeshAsset;
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

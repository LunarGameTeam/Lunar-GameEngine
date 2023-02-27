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
		mMaterialInstance(this)
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
	bool mCastShadow = true;
	render::RenderObject* mRO = nullptr;
	SharedPtr<MeshAsset> mMeshAsset;
	SharedPtr<render::MaterialTemplateAsset> mMaterialAsset;
	TPPtr<render::MaterialInstance> mMaterialInstance;
};

class GAME_API StaticMeshRenderer : public MeshRenderer
{
};


class GAME_API SkeletonMeshRenderer : public MeshRenderer
{
};

}

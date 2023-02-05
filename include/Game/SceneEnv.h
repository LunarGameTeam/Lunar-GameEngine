#pragma once

#include "Game/GameConfig.h"
#include "Core/Object/Entity.h"
#include "Core/Framework/Module.h"
#include "Graphics/RenderTypes.h"
#include "Graphics/Asset/MeshAsset.h"
#include "Graphics/Asset/MaterialTemplate.h"


namespace luna
{


class GAME_API SceneEnvComponent : public Component
{
	RegisterTypeEmbedd(SceneEnvComponent, Component)
public:
	SceneEnvComponent() :
		mSkyboxMesh(this),
		mSkyboxMaterial(this),
		mMaterialInstance(this)
	{

	}

	virtual ~SceneEnvComponent()
	{

	}
	void SetSkyboxMaterial(render::MaterialTemplateAsset* val);

	void SetAmbientColor(const LVector4f& ambient);


private:
	LVector4f mFogColor = LVector4f(0.1, 0.1, 0.1, 1);
	LVector4f mAmbientColor = LVector4f(0.1, 0.1, 0.1, 1);
	TPPtr<render::MeshAsset> mSkyboxMesh;
	TPPtr<render::MaterialTemplateAsset> mSkyboxMaterial;

private:
	TPPtr<render::MaterialInstance> mMaterialInstance;
};

}

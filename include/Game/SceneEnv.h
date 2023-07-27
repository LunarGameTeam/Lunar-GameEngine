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
		mMaterialInstance(this)
	{

	}

	virtual ~SceneEnvComponent()
	{

	}
	void SetSkyboxMaterial(graphics::MaterialTemplateAsset* val);

	void SetAmbientColor(const LVector4f& ambient);
	void SetGizmosEnable(bool val);
	void SetFogColor(const LVector4f& ambient);


private:
	bool      mDrawGizmos   = true;
	bool      mFogEnable    = true;
	LVector4f mFogColor     = LVector4f(0.1, 0.1, 0.1, 1);
	LVector4f mAmbientColor = LVector4f(0.1, 0.1, 0.1, 1);

	SharedPtr<graphics::MeshAsset>             mSkyboxMesh;
	SharedPtr<graphics::MaterialTemplateAsset> mSkyboxMaterial;

private:
	TPPtr<graphics::MaterialInstance> mMaterialInstance;
};

}

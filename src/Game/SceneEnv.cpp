#include "Game/SceneEnv.h"
#include "Core/Object/Transform.h"
#include "Game/Scene.h"
#include "Graphics/Renderer/RenderScene.h"

#include "Core/Memory/PtrBinding.h"

namespace luna
{

RegisterTypeEmbedd_Imp(SceneEnvComponent)
{
	cls->Ctor<SceneEnvComponent>();
	cls->Binding<SceneEnvComponent>();

	cls->BindingProperty<&SceneEnvComponent::mSkyboxMesh>("skybox_mesh")
		.Serialize();

	cls->BindingProperty<&SceneEnvComponent::mDrawGizmos>("draw_gizmos")
		.Setter<&SceneEnvComponent::SetGizmosEnable>()
		.Serialize();

	cls->BindingProperty<&SceneEnvComponent::mFogColor>("fog_color")
		.Setter<&SceneEnvComponent::SetFogColor>()
		.Serialize();

	cls->BindingProperty<&SceneEnvComponent::mAmbientColor>("ambient_color")
		.Setter<&SceneEnvComponent::SetAmbientColor>()
		.Serialize();

	cls->BindingProperty<&SceneEnvComponent::mSkyboxMesh>("skybox_mesh")
		.Serialize();

	cls->BindingProperty<&SceneEnvComponent::mSkyboxMaterial>("skybox_material")
		.Setter<&SceneEnvComponent::SetSkyboxMaterial>()
		.Serialize();

	BindingModule::Luna()->AddType(cls);
}

void SceneEnvComponent::SetSkyboxMaterial(render::MaterialTemplateAsset* val)
{
	if (GetScene() && val)
	{
		auto renderScene = GetScene()->GetRenderScene();
		renderScene->mSkyboxMaterial = val->GetDefaultInstance();
	}	
	mSkyboxMaterial = val;
}

void SceneEnvComponent::SetAmbientColor(const LVector4f& ambient)
{
	mAmbientColor = ambient;
	auto renderScene = GetScene()->GetRenderScene();
	renderScene->mAmbientColor = ambient;
	renderScene->SetSceneBufferDirty();
}

void SceneEnvComponent::SetGizmosEnable(bool val)
{
	mDrawGizmos = val;
	auto renderScene = GetScene()->GetRenderScene();
	renderScene->mDrawGizmos = mDrawGizmos;
	renderScene->SetSceneBufferDirty();
}

void SceneEnvComponent::SetFogColor(const LVector4f& ambient)
{

}

}
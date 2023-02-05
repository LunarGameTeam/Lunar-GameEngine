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

	cls->BindingProperty<&SceneEnvComponent::mSkyboxMaterial>("skybox_material")
		.Setter<&SceneEnvComponent::SetSkyboxMaterial>()
		.Serialize();

	BindingModule::Luna()->AddType(cls);
}

void SceneEnvComponent::SetSkyboxMaterial(render::MaterialTemplateAsset* val)
{
	if (GetScene())
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

}
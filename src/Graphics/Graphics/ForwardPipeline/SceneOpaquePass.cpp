#include "Graphics/ForwardPipeline/SceneOpaquePass.h"
#include "Graphics/ForwardPipeline/ForwardRenderData.h"
#include "Core/Asset/AssetModule.h"
#include "Graphics/FrameGraph/FrameGraphResource.h"
namespace luna::graphics
{

PARAM_ID(SceneBuffer);
PARAM_ID(ViewBuffer);
PARAM_ID(MaterialBuffer);
bool OpaquePassGenerator::CheckRenderObject(const RenderObject* curRo) const
{
	const RenderMeshBase* meshDataPointer = curRo->GetReadOnlyData<RenderMeshBase>();
	if (meshDataPointer == nullptr)
	{
		return false;
	}
	return true;
}

MaterialInstanceGraphBase* OpaquePassGenerator::SetMaterialByRenderObject(const RenderObject* curRo) const
{
	const RenderMeshBase* meshData = curRo->GetReadOnlyData<RenderMeshBase>();
	return meshData->mMaterial.Get();
}

OpaquePassGenerator::OpaquePassGenerator()
{
};

void OpaquePassGenerator::AddPassNode(FrameGraphBuilder* builder, RenderView* view)
{
	if (view->mViewType != RenderViewType::SceneView)
	{
		return;
	}
	RenderScene* renderScene = view->mOwnerScene;
	FGGraphDrawNode* node = builder->AddGraphDrawPass("Opaque");
	SceneRenderData* sceneRenderData = renderScene->RequireData<SceneRenderData>();
	if (sceneRenderData->mEnvTex == nullptr)
	{
		sceneRenderData->mEnvTex = LSharedPtr<TextureCube>(sAssetModule->LoadAsset<TextureCube>("/assets/built-in/Textures/Cubemap.dds"));
		sceneRenderData->mIrradianceTex = LSharedPtr<TextureCube>(sAssetModule->LoadAsset<TextureCube>("/assets/built-in/Textures/IrradianceMap.dds"));
		sceneRenderData->mLUTTex = LSharedPtr<Texture2D>(sAssetModule->LoadAsset<Texture2D>("/assets/built-in/Textures/brdf.dds"));
	}
	graphics::RenderViewParameterData* viewParamData = view->GetData<graphics::RenderViewParameterData>();
	graphics::RenderObjectDrawData* roParamData = renderScene->GetData<graphics::RenderObjectDrawData>();
	for (auto& eachCommand : mAllCommandsPool)
	{
		sceneRenderData->SetMaterialParameter(eachCommand.second.mDrawParameter.mMtl);
		viewParamData->SetMaterialParameter(eachCommand.second.mDrawParameter.mMtl);
		roParamData->SetMaterialParameter(eachCommand.second.mDrawParameter.mMtl);
	}
	GenerateNodeRenderTarget(builder, node, view);

	node->ExcuteFunc([this, renderScene](FrameGraphBuilder* builder, FGNode& node, RenderContext* device)
		{
			this->DrawCommandBatch();
		});
};

}


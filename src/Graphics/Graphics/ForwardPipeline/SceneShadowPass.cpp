#include "Graphics/ForwardPipeline/SceneShadowPass.h"
#include "Graphics/ForwardPipeline/ForwardRenderData.h"
#include "Core/Asset/AssetModule.h"
namespace luna::graphics
{

PARAM_ID(SceneBuffer);
PARAM_ID(ViewBuffer);
PARAM_ID(MaterialBuffer);

bool DirectionalLightShadowPassGenerator::CheckRenderObject(const RenderObject* curRo) const
{
	const RenderMeshBase* meshDataPointer = curRo->GetReadOnlyData<RenderMeshBase>();
	if (meshDataPointer == nullptr)
	{
		return false;
	}
	if (!meshDataPointer->mCastShadow)
	{
		return false;
	}
	return true;
}

MaterialInstanceGraphBase* DirectionalLightShadowPassGenerator::SetMaterialByRenderObject(const RenderObject* curRo) const
{
	return mShadowDefaultMtlInstance;
}

DirectionalLightShadowPassGenerator::DirectionalLightShadowPassGenerator()
{
	mShadowMtlAsset = sAssetModule->LoadAsset<MaterialGraphAsset>("/assets/built-in/Depth.mat");
	mShadowDefaultMtlInstance = dynamic_cast<MaterialInstanceGraphBase*>(mShadowMtlAsset->GetDefaultInstance());
};

void DirectionalLightShadowPassGenerator::AddPassNode(FrameGraphBuilder* builder, RenderView* view)
{
	if (view->mViewType != RenderViewType::ShadowMapView)
	{
		return;
	}
	RenderScene* renderScene = view->mOwnerScene;

	FGGraphDrawNode* node = builder->AddGraphDrawPass("Directional LightShadowmap");

	graphics::RenderViewParameterData* viewParamData = view->GetData<graphics::RenderViewParameterData>();

	graphics::RenderObjectDrawData*  roParamData = renderScene->GetData<graphics::RenderObjectDrawData>();

	viewParamData->SetMaterialParameter(mShadowDefaultMtlInstance);

	roParamData->SetMaterialParameter(mShadowDefaultMtlInstance);

	ShadowViewTargetData* viewRtData = view->RequireData<ShadowViewTargetData>();

	viewRtData->GenerateShadowRenderTarget(builder, node);

	node->ExcuteFunc([this](FrameGraphBuilder* builder, FGNode& node, RenderContext* device)
		{
			this->DrawCommandBatch();
		});
};
}


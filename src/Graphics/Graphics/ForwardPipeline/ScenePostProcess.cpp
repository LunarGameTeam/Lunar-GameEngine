#include "Graphics/ForwardPipeline/ScenePostProcess.h"
#include "Graphics/ForwardPipeline/ForwardRenderData.h"
#include "Core/Asset/AssetModule.h"
#include "Graphics/FrameGraph/FrameGraphResource.h"
#include "Graphics/Asset/MeshAssetUtils.h"
#include "Graphics/RenderModule.h"
namespace luna::graphics
{
bool PostProcessPassGenerator::CheckRenderObject(const RenderObject* curRo) const
{
	return false;
}

PostProcessPassGenerator::PostProcessPassGenerator()
{
	mGammaCorrectionMtlAsset = sAssetModule->LoadAsset<MaterialGraphAsset>("/assets/built-in/GammaCorrection.mat");
	mGammaCorrectionMtlInstance = dynamic_cast<MaterialInstanceGraphBase*>(mGammaCorrectionMtlAsset->GetDefaultInstance());
};

void PostProcessPassGenerator::AddPassNode(FrameGraphBuilder* builder, RenderView* view)
{
	if (view->mViewType != RenderViewType::SceneView)
	{
		return;
	}
	FGGraphDrawNode* node = builder->AddGraphDrawPass("GammaCorrection");
	ViewTargetData* viewRtData = view->RequireData<ViewTargetData>();
	//暂时应该没有pass依赖后处理，先把后处理的结果直接写入屏幕
	viewRtData->GenerateScreenRenderTarget(builder, node);

	node->ExcuteFunc([this](FrameGraphBuilder* builder, FGNode& node, RenderContext* device)
		{
			device->DrawFullScreen(this->mGammaCorrectionMtlInstance);
		});
};

}


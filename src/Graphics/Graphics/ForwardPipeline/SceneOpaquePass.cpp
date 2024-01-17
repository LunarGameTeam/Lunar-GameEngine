#include "Graphics/ForwardPipeline/SceneOpaquePass.h"
#include "Graphics/ForwardPipeline/ForwardRenderData.h"
#include "Core/Asset/AssetModule.h"
#include "Graphics/FrameGraph/FrameGraphResource.h"
namespace luna::graphics
{
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
	//绑定需要的输入数据
	SceneRenderData* sceneRenderData = renderScene->RequireData<SceneRenderData>();
	graphics::RenderViewParameterData* viewParamData = view->GetData<graphics::RenderViewParameterData>();
	graphics::RenderObjectDrawData* roParamData = renderScene->GetData<graphics::RenderObjectDrawData>();
	for (auto& eachCommand : mAllCommandsPool)
	{
		sceneRenderData->SetMaterialParameter(eachCommand.second.mDrawParameter.mMtl);
		viewParamData->SetMaterialParameter(eachCommand.second.mDrawParameter.mMtl);
		roParamData->SetMaterialParameter(eachCommand.second.mDrawParameter.mMtl);
	}
	//绑定rt输出
	ViewTargetData* viewRtData = view->RequireData<ViewTargetData>();
	viewRtData->GenerateOpaqueResultRenderTarget(builder, node);

	node->ExcuteFunc([this](FrameGraphBuilder* builder, FGNode& node, RenderContext* device)
		{
			this->DrawCommandBatch();
		});
};

}


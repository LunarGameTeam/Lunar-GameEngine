#include "Graphics/ForwardPipeline/SceneOverlayPass.h"
#include "Graphics/ForwardPipeline/ForwardRenderData.h"
#include "Core/Asset/AssetModule.h"
#include "Graphics/FrameGraph/FrameGraphResource.h"
#include "Graphics/Asset/MeshAssetUtils.h"
#include "Graphics/Renderer/RenderContext.h"
namespace luna::graphics
{

	bool SceneOverLayPassGenerator::CheckRenderObject(const RenderObject* curRo) const
	{
		return false;
	}

	SceneOverLayPassGenerator::SceneOverLayPassGenerator()
	{
		mDebugMeshLineData = sGlobelRenderResourceContext->GetAssetManager()->GetDebugMeshLineMesh();
		mDebugMeshData = sGlobelRenderResourceContext->GetAssetManager()->GetDebugMesh();
		mOverlayMtlAsset = sAssetModule->LoadAsset<MaterialGraphAsset>("/assets/built-in/Line.mat");
		mOverlayMtlInstance = dynamic_cast<MaterialInstanceGraphBase*>(mOverlayMtlAsset->GetDefaultInstance());
	};

	void SceneOverLayPassGenerator::AddPassNode(FrameGraphBuilder* builder, RenderView* view)
	{
		if (view->mViewType != RenderViewType::SceneView)
		{
			return;
		}
		FGGraphDrawNode* node = builder->AddGraphDrawPass("Overlay");
		graphics::RenderViewParameterData* viewParamData = view->GetData<graphics::RenderViewParameterData>();
		viewParamData->SetMaterialParameter(mOverlayMtlInstance);
		ViewTargetData* viewRtData = view->RequireData<ViewTargetData>();
		viewRtData->GenerateScreenRenderTarget(builder, node);

		node->ExcuteFunc([this](FrameGraphBuilder* builder, FGNode& node, RHICmdList* cmdlist)
			{
				sGlobelRenderCommondEncoder->DrawMesh(cmdlist,this->mDebugMeshLineData,this->mOverlayMtlInstance);
				sGlobelRenderCommondEncoder->DrawMesh(cmdlist, this->mDebugMeshData, this->mOverlayMtlInstance);
			});
	};

}


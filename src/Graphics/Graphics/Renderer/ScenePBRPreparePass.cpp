#include "Graphics/Renderer/MaterialInstance.h"
#include "Graphics/Asset/MeshAsset.h"
#include "Graphics/Asset/MaterialTemplate.h"

#include "Core/Asset/AssetModule.h"

#include "Graphics/FrameGraph/FrameGraph.h"
#include "Graphics/FrameGraph/FrameGraphNode.h"
#include "Graphics/FrameGraph/FrameGraphResource.h"

#include "Graphics/Renderer/RenderScene.h"
#include "Graphics/Renderer/RenderView.h"
#include "Graphics/Renderer/RenderLight.h"
#include "Graphics/RenderModule.h"


namespace luna::render
{

PARAM_ID(SceneBuffer);
PARAM_ID(ViewBuffer);
PARAM_ID(MaterialBuffer);

void PBRPreparePass(FrameGraphBuilder* builder, RenderView* view, RenderScene* renderScene)
{
	FGResourceView* sceneView = nullptr;
	SceneRenderData* data = renderScene->RequireData<SceneRenderData>();
	if (data->mLUTTex)
		return;
	{
		auto& node = builder->AddPass("LUTGenerate");
		node.SetupFunc(builder, [&](FrameGraphBuilder* builder, FGNode& node)
		{
			data->mLUTTex = builder->CreateTexture(
				512, 512, 1, 1,
			RHITextureFormat::R16G16B16A16_UNORM,
			RHIImageUsage::ColorAttachmentBit | RHIImageUsage::SampledBit,
			"LUT"
		);
		sceneView = node.AddRTV(data->mLUTTex, RHIViewDimension::TextureView2D);
		node.SetColorAttachment(sceneView, LoadOp::kLoad);
		});

		node.PreExecFunc([view, renderScene](FrameGraphBuilder* builder, FGNode& node)
		{

		});
		static MaterialTemplateAsset* lutMat = sAssetModule->LoadAsset<MaterialTemplateAsset>("/assets/built-in/LUT.mat");
		static MaterialInstance* lutMatInstance = lutMat->GetDefaultInstance();
		if (lutMatInstance)
			lutMatInstance->Ready();
		node.ExcuteFunc([view, renderScene, sceneView](FrameGraphBuilder* builder, FGNode& node, RenderContext* device)
		{
			PackedParams params;
		params.PushShaderParam(ParamID_SceneBuffer, renderScene->mSceneParamsBuffer);
		device->DrawMesh(sRenderModule->mFullscreenMesh, lutMatInstance, &params);
		});
	}
	{
		auto& node = builder->AddPass("IrradienceGenerate");
		node.SetupFunc(builder, [&](FrameGraphBuilder* builder, FGNode& node)
		{
			data->mLUTTex = builder->CreateTexture(
				512, 512, 1, 1,
			RHITextureFormat::R16G16B16A16_UNORM,
			RHIImageUsage::ColorAttachmentBit | RHIImageUsage::SampledBit,
			"Irradience"
		);
		sceneView = node.AddRTV(data->mLUTTex, RHIViewDimension::TextureView2D);
		node.SetColorAttachment(sceneView, LoadOp::kLoad);
		});

		node.PreExecFunc([view, renderScene](FrameGraphBuilder* builder, FGNode& node)
		{

		});
		static MaterialTemplateAsset* lutMat = sAssetModule->LoadAsset<MaterialTemplateAsset>("/assets/built-in/LUT.mat");
		static MaterialInstance* lutMatInstance = lutMat->GetDefaultInstance();
		if (lutMatInstance)
			lutMatInstance->Ready();
		node.ExcuteFunc([view, renderScene, sceneView](FrameGraphBuilder* builder, FGNode& node, RenderContext* device)
		{
			PackedParams params;
		params.PushShaderParam(ParamID_SceneBuffer, renderScene->mSceneParamsBuffer);
		device->DrawMesh(sRenderModule->mFullscreenMesh, lutMatInstance, &params);
		});
	}
}

}


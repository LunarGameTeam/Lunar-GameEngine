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
#include "Graphics/Renderer/RenderData.h"


namespace luna::render
{

PARAM_ID(SceneBuffer);
PARAM_ID(ViewBuffer);
PARAM_ID(MaterialBuffer);

void DirectionalLightShadowPass(FrameGraphBuilder* builder, RenderView* view, RenderScene* renderScene)
{
	if (!renderScene->mMainDirLight || !renderScene->mMainDirLight->mCastShadow)
		return;
	auto& node = builder->AddPass("Directional LightShadowmap");	
	node.SetupFunc(builder, [=](FrameGraphBuilder* builder, FGNode& node)
	{
		auto shadowData = view->RequireData<ViewShadowData>();

		FGTexture* color = builder->CreateTexture(
			1024, 1024, 1, 1,
			RHITextureFormat::R32_FLOAT, 
			RHIImageUsage::ColorAttachmentBit | RHIImageUsage::SampledBit,
			"DirecitonalShadowmap"
		);

		FGTexture* depth = builder->CreateTexture(
			1024, 1024, 1, 1,
			RHITextureFormat::D24_UNORM_S8_UINT,
			RHIImageUsage::DepthStencilBit,
			"DirecitonalShadowmapDepth"
		);

		shadowData->mDirectionLightShadowmap = color;

		auto colorView = node.AddRTV(color, RHIViewDimension::TextureView2D);
		auto depthView = node.AddDSV(depth);
		node.SetColorAttachment(colorView, LoadOp::kClear);
		node.SetDepthStencilAttachment(depthView);
	});

	static MaterialTemplateAsset* shadowMatAsset = sAssetModule->LoadAsset<MaterialTemplateAsset>("/assets/built-in/Depth.mat");
	static MaterialInstance* shadowMat = shadowMatAsset->GetDefaultInstance();
	if (shadowMat)
		shadowMat->Ready();

	node.ExcuteFunc([view, renderScene](FrameGraphBuilder* builder, FGNode& node, RenderContext* device)
	{
		ROArray& ROs = view->GetViewVisibleROs();
		PackedParams params;
		RHIResource* instancingBuffer = renderScene->mROIDInstancingBuffer->mRes;
		params.PushShaderParam(ParamID_SceneBuffer, renderScene->mSceneParamsBuffer);
		params.PushShaderParam(ParamID_ViewBuffer, renderScene->mMainDirLight->mParamBuffer);
		for (auto it : ROs)
		{
			RenderObject* ro = it;
			if (!it->mCastShadow)
				continue;
			it->mInstanceRes = instancingBuffer;
			device->DrawMeshInstanced(ro->mMesh, shadowMat, &params, ro->mInstanceRes, ro->mID);
		}
	});
}

void PointShadowPass(FrameGraphBuilder* builder, RenderView* view, RenderScene* renderScene)
{

	if (renderScene->mPointLights.size() == 0)
		return;
	bool hasShadow = false;
	for (PointLight* it : renderScene->mPointLights)
	{
		if (it->mCastShadow)
			hasShadow = true;
	}
	if (!hasShadow)
		return;
	for (uint32_t idx = 0; idx < 6; idx++)
	{
		auto& node = builder->AddPass(LString::Format("PointLightShadowmap{}", idx));

		node.SetupFunc(builder, [=](FrameGraphBuilder* builder, FGNode& node)
		{
			FGTexture* color = builder->CreateTexture(
				512, 512, 6, 1,
				RHITextureFormat::R32_FLOAT,
				RHIImageUsage::ColorAttachmentBit | RHIImageUsage::SampledBit,
				"PointShadowmap"
			);

			FGTexture* depth = builder->CreateTexture(
				512, 512, 6, 1,
				RHITextureFormat::D24_UNORM_S8_UINT,
				RHIImageUsage::DepthStencilBit,
				"PointShadowmapDepth"
			);

			assert(color);
			assert(depth);

			auto data = view->RequireData<ViewShadowData>();
			data->mPointShadowmap = color;

			auto colorView = node.AddRTV(color, RHIViewDimension::TextureView2D, idx, 1);
			auto depthView = node.AddDSV(depth);

			node.SetColorAttachment(colorView, LoadOp::kClear, StoreOp::kStore, LVector4f(1, 1, 1, 1));
			node.SetDepthStencilAttachment(depthView);
		});

		static MaterialTemplateAsset* shadowMatAsset = sAssetModule->LoadAsset<MaterialTemplateAsset>("/assets/built-in/Depth.mat");
		static MaterialInstance* shadowMat = shadowMatAsset->GetDefaultInstance();
		if (shadowMat)
			shadowMat->Ready();

		node.ExcuteFunc([view, renderScene, idx](FrameGraphBuilder* builder, FGNode& node, RenderContext* device)
		{
			for (PointLight* it : renderScene->mPointLights)
			{
				if (!it->mCastShadow)
					continue;
				ROArray& ROs = view->GetViewVisibleROs();
				PackedParams params;
				RHIResource* instancingBuffer = renderScene->mROIDInstancingBuffer->mRes;
				params.PushShaderParam(ParamID_SceneBuffer, renderScene->mSceneParamsBuffer);
				params.PushShaderParam(ParamID_ViewBuffer, it->mParamBuffer[idx]);
				for (auto it : ROs)
				{
					RenderObject* ro = it;
					if (!it->mCastShadow)
						continue;
					it->mInstanceRes = instancingBuffer;
					device->DrawMeshInstanced(ro->mMesh, shadowMat, &params, ro->mInstanceRes, ro->mID);
				}
			}

		});
	}
	
}

}


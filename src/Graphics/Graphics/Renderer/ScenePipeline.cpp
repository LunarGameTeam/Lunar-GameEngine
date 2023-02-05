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

void DirectionalLightShadowPass(FrameGraphBuilder* builder, RenderView* view, RenderScene* renderScene)
{

	if (!renderScene->mMainDirLight || !renderScene->mMainDirLight->mCastShadow)
		return;

	RHIResDesc shadowmapDesc;
	shadowmapDesc.mType = ResourceType::kTexture;
	shadowmapDesc.Width = 1024;
	shadowmapDesc.Height = 1024;
	shadowmapDesc.Format = RHITextureFormat::R32_FLOAT;
	shadowmapDesc.mImageUsage = RHIImageUsage::ColorAttachmentBit | RHIImageUsage::SampledBit;
	shadowmapDesc.DepthOrArraySize = 1;

	FGTexture* color = builder->CreateTexture("DirecitonalShadowmap", shadowmapDesc);

	shadowmapDesc.mImageUsage = RHIImageUsage::DepthStencilBit;
	shadowmapDesc.Format = RHITextureFormat::FORMAT_D24_UNORM_S8_UINT;
	FGTexture* depth = builder->CreateTexture("DirecitonalShadowmapDepth", shadowmapDesc);

	auto& node = builder->AddPass("Direction LightShadowmap");
	node.SetupFunc(builder, [=](FrameGraphBuilder* builder, FGNode& node)
	{
		assert(color);
		assert(depth);

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

	RHIResDesc shadowmapDesc;
	shadowmapDesc.mType = ResourceType::kTexture;
	shadowmapDesc.Width = 512;
	shadowmapDesc.Height = 512;
	shadowmapDesc.Format = RHITextureFormat::R32_FLOAT;
	shadowmapDesc.mImageUsage = RHIImageUsage::ColorAttachmentBit | RHIImageUsage::SampledBit;
	shadowmapDesc.DepthOrArraySize = 6;


	FGTexture* color = builder->CreateTexture("Shadowmap", shadowmapDesc);

	shadowmapDesc.mImageUsage = RHIImageUsage::DepthStencilBit;
	shadowmapDesc.Format = RHITextureFormat::FORMAT_D24_UNORM_S8_UINT;
	FGTexture* depth = builder->CreateTexture("ShadowmapDepth", shadowmapDesc);

	for (uint32_t idx = 0; idx < 6; idx++)
	{
		auto& node = builder->AddPass(LString::Format("PointLightShadowmap{}", idx));
		node.SetupFunc(builder, [=](FrameGraphBuilder* builder, FGNode& node)
		{
			assert(color);
			assert(depth);

			auto colorView = node.AddRTV(color, RHIViewDimension::TextureView2D, idx, 1);
			auto depthView = node.AddDSV(depth);

			node.SetColorAttachment(colorView, LoadOp::kClear, StoreOp::kStore, LVector4f(1, 1, 1, 1));
			node.SetDepthStencilAttachment(depthView);
		});

		node.PreExecFunc([view, renderScene](FrameGraphBuilder* builder, FGNode& node)
		{

		});

		static MaterialTemplateAsset* shadowMatAsset = sAssetModule->LoadAsset<MaterialTemplateAsset>("/assets/built-in/Depth.mat");
		static MaterialInstance* shadowMat = shadowMatAsset->GetDefaultInstance();
		if (shadowMat)
			shadowMat->Ready();

		node.ExcuteFunc([view, renderScene, idx](FrameGraphBuilder* builder, FGNode& node, RenderContext* device)
		{
			if (renderScene->mPointLights.size() == 0)
				return;

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

void OpaquePass(FrameGraphBuilder* builder, RenderView* view, RenderScene* renderScene)
{

	auto& node = builder->AddPass("Opaque");
	FGResourceView* shadowmapView = nullptr;
	FGResourceView* directionalShadowmapView = nullptr;
	node.SetupFunc(builder, [&](FrameGraphBuilder* builder, FGNode& node)
	{
		LString rtName = "MainColor";
		LString rtDepthName = "MainDepth";
		RHIResourcePtr colorTexture = view->GetRenderTarget() ? view->GetRenderTarget()->mColorTexture : sRenderModule->mMainRT->mColorTexture;
		RHIResourcePtr depthTexture = view->GetRenderTarget() ? view->GetRenderTarget()->mDepthTexture : sRenderModule->mMainRT->mDepthTexture;
		builder->BindExternalTexture(rtName, colorTexture);
		builder->BindExternalTexture(rtDepthName, depthTexture);

		FGTexture* color = builder->GetTexture(rtName);
		FGTexture* depth = builder->GetTexture(rtDepthName);

		assert(color);
		assert(depth);

		auto colorView = node.AddRTV(color, RHIViewDimension::TextureView2D);
		auto depthView = node.AddDSV(depth);

		FGTexture* shadowmap = builder->GetTexture("Shadowmap");
		FGTexture* directionalShadowmap = builder->GetTexture("DirecitonalShadowmap");
		if (shadowmap)
		{
			shadowmapView = node.AddSRV(shadowmap, RHIViewDimension::TextureView2D, 0 , 6);
		}
		if (directionalShadowmap)
		{
			directionalShadowmapView = node.AddSRV(directionalShadowmap, RHIViewDimension::TextureView2D, 0, 1);
		}
		
		node.SetColorAttachment(colorView);
		node.SetDepthStencilAttachment(depthView);
	});
	node.PreExecFunc([view, renderScene](FrameGraphBuilder* builder, FGNode& node) 
	{

	});
	static MeshAsset* sSkyboxMesh = sAssetModule->LoadAsset<MeshAsset>("/assets/built-in/Geometry/InsideSphere.lmesh");	
	

	node.ExcuteFunc([view, renderScene, shadowmapView, directionalShadowmapView](FrameGraphBuilder* builder, FGNode& node, RenderContext* device)
	{
		ROArray& ROs = view->GetViewVisibleROs();
		PackedParams params;
		
		PARAM_ID(_MainTex);
		PARAM_ID(_ShadowMap);
		PARAM_ID(_DirectionLightShadowMap);
		RHIResource* instancingBuffer = renderScene->mROIDInstancingBuffer->mRes;

		params.Clear();
		//»­Ìì¿ÕºÐ×Ó
		if (renderScene->mSkyboxMaterial != nullptr)
		{
			auto skyboxSubmesh = sSkyboxMesh->GetSubMeshAt(0);
			MaterialInstance* mat = renderScene->mSkyboxMaterial;
			params.PushShaderParam(ParamID_SceneBuffer, renderScene->mSceneParamsBuffer);
			params.PushShaderParam(ParamID_ViewBuffer, view->mViewBuffer);
			if (mat->mParamsView)
			{
				params.PushShaderParam(ParamID_MaterialBuffer, mat->mParamsView);//Push Material BindPoint
			}
			for (auto matParam : mat->mMaterialParams.mParams)
			{
				if (matParam.second)
					params.PushShaderParam(matParam.first, matParam.second);
			}
			device->DrawMesh(skyboxSubmesh, mat, &params);
		}

		for (auto it : ROs)
		{
			params.Clear();
			MaterialInstance* mat = it->mMaterial;
			RenderObject* ro = it;
			if (mat)
			{
				mat->Ready();
			}
			it->mInstanceRes = instancingBuffer;
			ShaderAsset* shader = mat->GetShaderAsset();
			if(shadowmapView)
				params.PushShaderParam(ParamID__ShadowMap, shadowmapView->mRHIView);
			if(directionalShadowmapView)
				params.PushShaderParam(ParamID__DirectionLightShadowMap, directionalShadowmapView->mRHIView);
			params.PushShaderParam(ParamID_SceneBuffer, renderScene->mSceneParamsBuffer);
			params.PushShaderParam(ParamID_ViewBuffer, view->mViewBuffer);
			if (mat->mParamsView)
			{
				params.PushShaderParam(ParamID_MaterialBuffer, mat->mParamsView);//Push Material BindPoint
			}
			for (auto matParam : mat->mMaterialParams.mParams)
			{
				if(matParam.second)
					params.PushShaderParam(matParam.first, matParam.second);
			}
			
			device->DrawRenderOBject(ro, mat, &params);
		}
		
	});
}

void OverlayPass(FrameGraphBuilder* builder, RenderView* view, RenderScene* renderScene)
{
	auto& node = builder->AddPass("Overlay");
	FGResourceView* shadowmapView = nullptr;

	node.SetupFunc(builder, [&](FrameGraphBuilder* builder, FGNode& node)
	{
		LString rtName = "MainColor";
		LString rtDepthName = "MainDepth";
		RHIResourcePtr colorTexture = view->GetRenderTarget() ? view->GetRenderTarget()->mColorTexture : sRenderModule->mMainRT->mColorTexture;
		RHIResourcePtr depthTexture = view->GetRenderTarget() ? view->GetRenderTarget()->mDepthTexture : sRenderModule->mMainRT->mDepthTexture;
		builder->BindExternalTexture(rtName, colorTexture);
		builder->BindExternalTexture(rtDepthName, depthTexture);

		FGTexture* color = builder->GetTexture(rtName);
		FGTexture* depth = builder->GetTexture(rtDepthName);

		assert(color);
		assert(depth);

		auto colorView = node.AddRTV(color, RHIViewDimension::TextureView2D);
		auto depthView = node.AddDSV(depth);

		node.SetColorAttachment(colorView, LoadOp::kLoad);
		node.SetDepthStencilAttachment(depthView);
	});

	node.PreExecFunc([view, renderScene](FrameGraphBuilder* builder, FGNode& node)
	{

	});
	static MaterialTemplateAsset* debugMatAsset = sAssetModule->LoadAsset<MaterialTemplateAsset>("/assets/built-in/Line.mat");
	static MaterialInstance* debugMat = debugMatAsset->GetDefaultInstance();
	if (debugMat)
		debugMat->Ready();
	node.ExcuteFunc([view, renderScene, shadowmapView](FrameGraphBuilder* builder, FGNode& node, RenderContext* device) 
	{
		if (!renderScene->mDrawGizmos)
			return;
		ROArray& ROs = view->GetViewVisibleROs();
		PackedParams params;
		PARAM_ID(MaterialBuffer);
		PARAM_ID(_MainTex);
		PARAM_ID(_ShadowMap);
		params.PushShaderParam(ParamID_SceneBuffer, renderScene->mSceneParamsBuffer);
		params.PushShaderParam(ParamID_ViewBuffer, view->mViewBuffer);
		RHIResource* instancingBuffer = renderScene->mROIDInstancingBuffer->mRes;
		if(renderScene->mDebugMeshLine->mVertexData.size())
			device->DrawMesh(renderScene->mDebugMeshLine, debugMat, &params);
		if (renderScene->mDebugMesh->mVertexData.size())
			device->DrawMesh(renderScene->mDebugMesh, debugMat, &params);
	});
}

}


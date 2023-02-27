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

void OpaquePass(FrameGraphBuilder* builder, RenderView* view, RenderScene* renderScene)
{
	
	auto& node = builder->AddPass("Opaque");
	
	FGResourceView* shadowmapView = nullptr;	
	FGResourceView* directionalShadowmapView = nullptr;
	FGResourceView* lutView = nullptr;
	FGResourceView* envView = nullptr;
	FGResourceView* irradianceView = nullptr;
	node.SetupFunc(builder, [&](FrameGraphBuilder* builder, FGNode& node)
	{
		LString rtName = "MainColor";
		LString rtDepthName = "MainDepth";

		SceneRenderData* data = renderScene->RequireData<SceneRenderData>();
		
		RHIResourcePtr colorTexture = view->GetRenderTarget() ? view->GetRenderTarget()->mColorTexture : sRenderModule->mMainRT->mColorTexture;
		RHIResourcePtr depthTexture = view->GetRenderTarget() ? view->GetRenderTarget()->mDepthTexture : sRenderModule->mMainRT->mDepthTexture;

		data->mSceneColor = builder->CreateTexture(
			colorTexture->GetDesc().Width, colorTexture->GetDesc().Height, 1, 1,
			RHITextureFormat::R16G16B16A16_UNORM,
			RHIImageUsage::ColorAttachmentBit | RHIImageUsage::SampledBit,
			"SceneColor"
		);

		data->mSceneDepth = builder->CreateTexture(
			depthTexture->GetDesc().Width, depthTexture->GetDesc().Height, 1, 1,
			depthTexture->GetDesc().Format, RHIImageUsage::DepthStencilBit,
			"SceneDepth"
		);

		assert(data->mSceneColor);
		assert(data->mSceneDepth);

		auto colorView = node.AddRTV(data->mSceneColor, RHIViewDimension::TextureView2D);
		auto depthView = node.AddDSV(data->mSceneDepth);

		ViewShadowData* shadowData = view->GetData<ViewShadowData>();
		lutView = node.AddSRV(data->mLUTTex, RHIViewDimension::TextureView2D);
		irradianceView = node.AddSRV(data->mIrradianceTex, RHIViewDimension::TextureViewCube, 0, 6);
		envView = node.AddSRV(data->mEnvTex, RHIViewDimension::TextureViewCube, 0 ,6);
		if (shadowData)
		{
			FGTexture* shadowmap = shadowData->mPointShadowmap;
			FGTexture* directionalShadowmap = shadowData->mDirectionLightShadowmap;
			if (shadowmap)
			
				shadowmapView = node.AddSRV(shadowmap, RHIViewDimension::TextureView2DArray , 0, 6);
			
			if (directionalShadowmap)
			
				directionalShadowmapView = node.AddSRV(directionalShadowmap, RHIViewDimension::TextureView2D, 0, 1);
			
		}
		else
		{
			FGTexture* white = builder->BindExternalTexture(sRenderModule->mDefaultWhiteTexture->GetRHITexture(), "white");
			shadowmapView = node.AddSRV(white, RHIViewDimension::TextureView2DArray, 0, 1);
			directionalShadowmapView = node.AddSRV(white, RHIViewDimension::TextureView2D, 0, 1);
		}		

		node.SetColorAttachment(colorView);
		node.SetDepthStencilAttachment(depthView);
	});
	static auto sSkyboxMesh = sAssetModule->LoadAsset<MeshAsset>("/assets/built-in/Geometry/Sphere.lmesh");	
	

	node.ExcuteFunc([=](FrameGraphBuilder* builder, FGNode& node, RenderContext* device)
	{
		ROArray& ROs = view->GetViewVisibleROs();

		SceneRenderData* data = renderScene->RequireData<SceneRenderData>();
		PackedParams params;		
		PARAM_ID(_MainTex);
		PARAM_ID(_LUTTex);
		PARAM_ID(_ShadowMap);
		PARAM_ID(_EnvTex);
		PARAM_ID(_DirectionLightShadowMap);
		PARAM_ID(_IrradianceTex);
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
				mat->Ready();

			it->mInstanceRes = instancingBuffer;
			ShaderAsset* shader = mat->GetShaderAsset();
			
			if(shadowmapView)
				params.PushShaderParam(ParamID__ShadowMap, shadowmapView->mRHIView);

			if(directionalShadowmapView)
				params.PushShaderParam(ParamID__DirectionLightShadowMap, directionalShadowmapView->mRHIView);

			params.PushShaderParam(ParamID__EnvTex, envView->mRHIView);
			params.PushShaderParam(ParamID__LUTTex, lutView->mRHIView);
			params.PushShaderParam(ParamID__IrradianceTex, irradianceView->mRHIView);
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

}


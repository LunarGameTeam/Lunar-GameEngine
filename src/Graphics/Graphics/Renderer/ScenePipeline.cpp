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

void ShadowPass(FrameGraphBuilder* builder, RenderView* view, RenderScene* renderScene)
{
	auto& node = builder->AddPass("Shadowmap");

	node.SetupFunc(builder, [=](FrameGraphBuilder* builder, FGNode& node)
	{
		RHIResDesc shadowmapDesc;
		shadowmapDesc.mType = ResourceType::kTexture;
		shadowmapDesc.Width = 1024;
		shadowmapDesc.Height = 1024;
		shadowmapDesc.Format = RHITextureFormat::FORMAT_R8G8BB8A8_UNORM;
		shadowmapDesc.mImageUsage = RHIImageUsage::ColorAttachmentBit;

		FGTexture* color = builder->CreateTexture("Shadowmap", shadowmapDesc);

		shadowmapDesc.mImageUsage = RHIImageUsage::DepthStencilBit;
		shadowmapDesc.Format = RHITextureFormat::FORMAT_D24_UNORM_S8_UINT;
		FGTexture* depth = builder->CreateTexture("ShadowmapDepth", shadowmapDesc);		
		

		assert(color);
		assert(depth);

		ViewDesc srvDesc;
		srvDesc.mViewType = RHIViewType::kTexture;
		srvDesc.mViewDimension = RHIViewDimension::TextureView2D;

		ViewDesc rtvDesc;
		rtvDesc.mViewType = RHIViewType::kRenderTarget;
		rtvDesc.mViewDimension = RHIViewDimension::TextureView2D;

		ViewDesc dsvDesc;
		dsvDesc.mViewType = RHIViewType::kDepthStencil;
		dsvDesc.mViewDimension = RHIViewDimension::TextureView2D;

		auto colorView = node.AddRTV(color, rtvDesc);
		auto depthView = node.AddDSV(depth, dsvDesc);

		node.SetColorAttachment(colorView);
		node.SetDepthStencilAttachment(depthView);
	});
	node.PreExecFunc([view, renderScene](FrameGraphBuilder* builder, FGNode& node)
	{

	});
	static MaterialTemplateAsset* shadowMat = sAssetModule->LoadAsset<MaterialTemplateAsset>("/assets/built-in/Depth.mat");
	
	node.ExcuteFunc([view, renderScene](FrameGraphBuilder* builder, FGNode& node, RenderDevice* device) {

		ROArray& ROs = view->GetViewVisibleROs();
		static PackedParams params;
		PARAM_ID(SceneBuffer);		
		PARAM_ID(ViewBuffer);
		RHIResource* instancingBuffer = renderScene->mROIDInstancingBuffer->mRes;
		MaterialInstance* mat = shadowMat->GetDefaultInstance();
		if (mat)		
			mat->Ready();

		params.PushShaderParam(ParamID_SceneBuffer, renderScene->mSceneParamsBuffer);
		params.PushShaderParam(ParamID_ViewBuffer, renderScene->mMainDirLight->mParamBuffer.get());

		for (auto it : ROs)
		{
			params.Clear();			
			RenderObject* ro = it;			
			if(!it->mCastShadow)
				continue;
			it->mInstanceRes = instancingBuffer;
			ShaderAsset* shader = mat->GetShaderAsset();				
			device->DrawMeshInstanced(ro->mMesh, mat, &params, ro->mInstanceRes, ro->mID);
		}
	});
}

void OpaquePass(FrameGraphBuilder* builder, RenderView* view, RenderScene* renderScene)
{

	auto& node = builder->AddPass("Opaque");

	node.SetupFunc(builder, [=](FrameGraphBuilder* builder, FGNode& node)
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

		ViewDesc srvDesc;
		srvDesc.mViewType = RHIViewType::kTexture;
		srvDesc.mViewDimension = RHIViewDimension::TextureView2D;

		ViewDesc rtvDesc;
		rtvDesc.mViewType = RHIViewType::kRenderTarget;
		rtvDesc.mViewDimension = RHIViewDimension::TextureView2D;

		ViewDesc dsvDesc;
		dsvDesc.mViewType = RHIViewType::kDepthStencil;
		dsvDesc.mViewDimension = RHIViewDimension::TextureView2D;

		auto colorView = node.AddRTV(color, rtvDesc);
		auto depthView = node.AddDSV(depth, dsvDesc);
		node.SetColorAttachment(colorView);
		node.SetDepthStencilAttachment(depthView);
	});
	node.PreExecFunc([view, renderScene](FrameGraphBuilder* builder, FGNode& node) 
	{

	});
	node.ExcuteFunc([view, renderScene](FrameGraphBuilder* builder, FGNode& node, RenderDevice* device) {

		ROArray& ROs = view->GetViewVisibleROs();
		static PackedParams params;
		PARAM_ID(SceneBuffer);
		PARAM_ID(ViewBuffer);
		PARAM_ID(MaterialBuffer);
		RHIResource* instancingBuffer = renderScene->mROIDInstancingBuffer->mRes;
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

}

}


#include "Graphics/Renderer/MaterialInstance.h"
#include "Graphics/Asset/MeshAsset.h"

#include "Core/Asset/AssetModule.h"

#include "Graphics/FrameGraph/FrameGraph.h"
#include "Graphics/FrameGraph/FrameGraphNode.h"
#include "Graphics/FrameGraph/FrameGraphResource.h"

#include "Graphics/Renderer/RenderScene.h"
#include "Graphics/Renderer/RenderView.h"
#include "Graphics/RenderModule.h"


namespace luna::render
{

void BuildRenderPass(FrameGraphBuilder* builder, RenderView* view, RenderScene* renderScene)
{

	//��lambda
	auto& node = builder->AddPass("LightingPass");

	node.SetupFunc(builder, [=](FrameGraphBuilder* builder, FGNode& node)
	{
		LString rtName = "MainColor";
	LString rtDepthName = "MainDepth";
	RHIResourcePtr colorTexture = view->GetRenderTarget() ? view->GetRenderTarget()->mColorTexture : sRenderModule->mMainRT->mColorTexture;
	RHIResourcePtr depthTexture = view->GetRenderTarget() ? view->GetRenderTarget()->mDepthTexture : sRenderModule->mMainRT->mDepthTexture;
	builder->BindExternalTexture(rtName, colorTexture);
	builder->BindExternalTexture(rtDepthName, depthTexture);

	//builder->BindExternalTexture(mShadowMapDsName, view->GetRenderTarget()->mDepthTexture);

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

	node.ExcuteFunc([view, renderScene](FrameGraphBuilder* builder, FGNode& node, RenderDevice* device) {

		static PackedParams params;
	PARAM_ID(SceneBuffer);
	PARAM_ID(ObjectBuffer);
	PARAM_ID(ViewBuffer);
	PARAM_ID(MaterialBuffer);
	if (renderScene->GetRenderObjects().size() == 0)
	{
		return;
	}
	//Ŀǰ���е�objectֻ���һ��
	//RHIResource* instancingBuffer = device->CreateInstancingBufferByRenderObjects(renderScene->GetRenderObjects());
	for (auto it : renderScene->GetRenderObjects())
	{
		params.Clear();
		MaterialInstance* mat = it->mMaterial;
		RenderObject* ro = it;
		if (mat)
		{
			mat->Ready();
		}
		ShaderAsset* shader = mat->GetShaderAsset();
		params.PushShaderParam(ParamID_SceneBuffer, renderScene->mSceneBufferView);
		params.PushShaderParam(ParamID_ObjectBuffer, renderScene->mROBufferView);
		params.PushShaderParam(ParamID_ViewBuffer, view->GetPerViewBufferView());
		params.PushShaderParam(ParamID_MaterialBuffer, mat->mParamsView);
		for (auto matParam : mat->mMaterialParams.mParams)
		{
			params.PushShaderParam(matParam.first, matParam.second);
		}
		LArray<RenderObject*> cacheRenderObjects;
		cacheRenderObjects.push_back(it);
		RHIResource* instancingBuffer = device->CreateInstancingBufferByRenderObjects(cacheRenderObjects);
		device->DrawRenderOBject(ro, mat, &params, instancingBuffer, 1);
	}
	});
}


}

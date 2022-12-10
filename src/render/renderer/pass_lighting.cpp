#include "render/renderer/pass_lighting.h"

#include "core/asset/asset_module.h"
#include "render/render_module.h"
#include "core/reflection/type_traits.h"


namespace luna::render
{


void LightingPass::BuildRenderPass(FrameGraphBuilder* builder, RenderView* view, RenderScene* renderScene)
{
	RenderPass::BuildRenderPass(builder, view, renderScene);

	//用lambda
	auto& node = builder->AddPass("LightingPass");

	node.SetupFunc(builder, [=](FrameGraphBuilder* builder, FGNode& node){
			LString rtName = "MainColor";
			LString rtDepthName = "MainDepth";
			builder->BindExternalTexture(rtName, sRenderModule->mMainRT->mColorTexture);
			builder->BindExternalTexture(rtDepthName, sRenderModule->mMainRT->mDepthTexture);

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
	
	node.ExcuteFunc([view, renderScene](FrameGraphBuilder* builder, FGNode& node, RenderDevice* device){

		static PackedParams params;
		PARAM_ID(SceneBuffer);
		PARAM_ID(ObjectBuffer);
		PARAM_ID(ViewBuffer);
		PARAM_ID(MaterialBuffer);
		if (renderScene->GetRenderObjects().size() == 0)
		{
			return;
		}
		//目前所有的object只打成一组
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
			LVector<RenderObject*> cacheRenderObjects;
			cacheRenderObjects.push_back(it);
			RHIResource* instancingBuffer = device->CreateInstancingBufferByRenderObjects(cacheRenderObjects);
			device->DrawRenderOBject(ro, shader, &params, instancingBuffer,1);
		}
	});
}

}

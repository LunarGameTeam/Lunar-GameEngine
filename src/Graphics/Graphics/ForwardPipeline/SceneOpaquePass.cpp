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

#include "Graphics/ForwardPipeline/ForwardRenderData.h"

namespace luna::render
{

PARAM_ID(SceneBuffer);
PARAM_ID(ViewBuffer);
PARAM_ID(MaterialBuffer);
class OpaquePassProcessor : public RoPassProcessorBase
{
public:
	OpaquePassProcessor(RenderScene* scene, MeshRenderPass passType);
	void AddRenderObject(RenderObject* renderObject) override;
};
OpaquePassProcessor::OpaquePassProcessor(RenderScene* scene, MeshRenderPass passType) : RoPassProcessorBase(scene, passType)
{

}
void OpaquePassProcessor::AddRenderObject(RenderObject* renderObject)
{
	BuildMeshRenderCommands(renderObject, renderObject->mMaterial);
}
RoPassProcessorBase* OpaquePassProcessorCreateFunction(RenderScene* scene, MeshRenderPass passType)
{
	return new OpaquePassProcessor(scene, passType);
}
void OpaquePass(FrameGraphBuilder* builder, RenderView* view, RenderScene* renderScene)
{
	
	auto& node = builder->AddPass("Opaque");
	
	FGResourceView* shadowmapView = nullptr;	
	FGResourceView* directionalShadowmapView = nullptr;
	FGResourceView* lutView = nullptr;
	FGResourceView* envView = nullptr;
	FGResourceView* irradianceView = nullptr;

	SceneRenderData* data = renderScene->RequireData<SceneRenderData>();
	ViewShadowData* shadowData = view->GetData<ViewShadowData>();

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
	if(data->mLUTTex)
		lutView = node.AddSRV(data->mLUTTex, RHIViewDimension::TextureView2D);
	else
		lutView = node.AddSRV(builder->GetTexture("white"), RHIViewDimension::TextureView2D);

	if(data->mIrradianceTex)
		irradianceView = node.AddSRV(data-> mIrradianceTex, RHIViewDimension::TextureViewCube, 0, 6);


	if(data->mEnvTex)
		envView = node.AddSRV(data->mEnvTex, RHIViewDimension::TextureViewCube, 0, 6);


	if (shadowData)
	{
		if (shadowData->mPointShadowmap)
			shadowmapView = node.AddSRV(shadowData->mPointShadowmap, RHIViewDimension::TextureView2DArray, 0, 6);
		if (shadowData->mDirectionLightShadowmap)
			directionalShadowmapView = node.AddSRV(shadowData->mDirectionLightShadowmap, RHIViewDimension::TextureView2D, 0, 1);
	}
	else
	{
		FGTexture* white = builder->BindExternalTexture(sRenderModule->mDefaultWhiteTexture->GetRHITexture(), "white");
		shadowmapView = node.AddSRV(white, RHIViewDimension::TextureView2DArray, 0, 1);
		directionalShadowmapView = node.AddSRV(white, RHIViewDimension::TextureView2D, 0, 1);
	}

	node.SetColorAttachment(colorView);
	node.SetDepthStencilAttachment(depthView);

	static auto sSkyboxMesh = sAssetModule->LoadAsset<MeshAsset>("/assets/built-in/Geometry/Sphere.lmesh");	
	static int32_t sSkyboxMeshID = renderScene->mSceneDataGpu.AddMeshData(sSkyboxMesh->GetSubMeshAt(0));
	static RenderMeshBase* sSkyboxRenderMesh = renderScene->mSceneDataGpu.GetMeshData(sSkyboxMeshID);

	node.ExcuteFunc([=](FrameGraphBuilder* builder, FGNode& node, RenderContext* device)
	{
		//ROArray& ROs = view->GetViewVisibleROs();
	
		PARAM_ID(_MainTex);
		PARAM_ID(_LUTTex);
		PARAM_ID(_ShadowMap);
		PARAM_ID(_EnvTex);
		PARAM_ID(_DirectionLightShadowMap);
		PARAM_ID(_IrradianceTex);

		//»­Ìì¿ÕºÐ×Ó
		if (renderScene->mSkyboxMaterial)
		{
			MaterialInstance* mat = renderScene->mSkyboxMaterial;
			mat->SetShaderInput(ParamID_SceneBuffer, renderScene->mSceneParamsBuffer->mView);
			mat->SetShaderInput(ParamID_ViewBuffer, view->mViewBuffer->mView);
			device->DrawMesh(sSkyboxRenderMesh, mat, nullptr);
		}

		RHIResource* instancingBuffer = renderScene->mROIDInstancingBuffer->mRes;
		std::unordered_map<luna::render::ShaderParamID, luna::render::RHIView*> shaderBindingParam;
		if (shadowmapView)
			shaderBindingParam.insert({ ParamID__ShadowMap ,shadowmapView->mRHIView.get() });

		if (directionalShadowmapView)
			shaderBindingParam.insert({ ParamID__DirectionLightShadowMap ,directionalShadowmapView->mRHIView.get() });

		if (envView)
			shaderBindingParam.insert({ ParamID__EnvTex ,envView->mRHIView.get() });
		if (lutView)
			shaderBindingParam.insert({ ParamID__LUTTex ,lutView->mRHIView.get() });
		if (irradianceView)
			shaderBindingParam.insert({ ParamID__IrradianceTex ,irradianceView->mRHIView.get() });
		view->mMeshRenderCommandsPass[MeshRenderPass::LightingPass].DrawAllCommands(shaderBindingParam);
		
	});
}

}


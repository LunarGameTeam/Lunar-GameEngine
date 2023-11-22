#include "Graphics/Renderer/MaterialInstance.h"
#include "Graphics/Asset/MeshAsset.h"
#include "Graphics/Asset/MaterialTemplate.h"

#include "Core/Asset/AssetModule.h"

#include "Graphics/FrameGraph/FrameGraph.h"
#include "Graphics/FrameGraph/FrameGraphNode.h"
#include "Graphics/FrameGraph/FrameGraphResource.h"
#include "Graphics/FrameGraph/FrameGraphPassGenerator.h"

#include "Graphics/Renderer/RenderScene.h"
#include "Graphics/Renderer/RenderView.h"
#include "Graphics/Renderer/RenderLight.h"
#include "Graphics/RenderModule.h"
#include "Graphics/Renderer/RenderData.h"

#include "Graphics/ForwardPipeline/ForwardRenderData.h"

namespace luna::graphics
{

PARAM_ID(SceneBuffer);
PARAM_ID(ViewBuffer);
PARAM_ID(MaterialBuffer);

class DirectionalLightShadowPassGenerator : public FrameGraphPassGenerator
{
	SharedPtr<MaterialTemplateAsset> mShadowMtlAsset = nullptr;
	MaterialInstance* mShadowDefaultMtlInstance = nullptr;
public:
	DirectionalLightShadowPassGenerator(){};

	void AddPassNode(FrameGraphBuilder* builder, RenderView* view, RenderScene* renderScene) override;
private:
	bool CheckRenderObject(const RenderObject* curRo) const override;
};

bool DirectionalLightShadowPassGenerator::CheckRenderObject(const RenderObject* curRo) const
{
	const RenderMeshBase* meshDataPointer = curRo->GetReadOnlyData<RenderMeshBase>();
	if (meshDataPointer == nullptr)
	{
		return false;
	}
	if (!meshDataPointer->mCastShadow)
	{
		return false;
	}
	return true;
}

void DirectionalLightShadowPassGenerator::AddPassNode(FrameGraphBuilder* builder, RenderView* view, RenderScene* renderScene)
{
	if (view->mViewType != RenderViewType::ShadowMapView)
	{
		return;
	}
	if (mShadowMtlAsset == nullptr)
	{
		mShadowMtlAsset = sAssetModule->LoadAsset<MaterialTemplateAsset>("/assets/built-in/Depth.mat");
		mShadowDefaultMtlInstance = mShadowMtlAsset->GetDefaultInstance();
		if (mShadowDefaultMtlInstance)
			mShadowDefaultMtlInstance->Ready();
	}
	FGGraphDrawNode* node = builder->AddGraphDrawPass("Directional LightShadowmap");
	ViewShadowData* shadowData = view->RequireData<ViewShadowData>();
	if (shadowData->mLightShadowDepth == nullptr)
	{
		shadowData->mLightShadowmap = builder->CreateCommon2DTexture(
			"DirecitonalShadowmap",
			1024, 1024,
			RHITextureFormat::R32_FLOAT,
			RHIImageUsage::ColorAttachmentBit | RHIImageUsage::SampledBit
		);

		shadowData->mLightShadowDepth = builder->CreateCommon2DTexture(
			"DirecitonalShadowmapDepth",
			1024, 1024,
			RHITextureFormat::D24_UNORM_S8_UINT,
			RHIImageUsage::DepthStencilBit
		);
	}
	
	MeshDrawCommandBatch newMesh;
	newMesh.mMtl = mShadowDefaultMtlInstance;
	view->SetMaterialViewParameter(mShadowDefaultMtlInstance);
	renderScene->SetMaterialSceneParameter(mShadowDefaultMtlInstance);
	for (RenderObject* curRo : mRoQueue)
	{
		const RenderMeshBase* meshDataPointer = curRo->GetReadOnlyData<RenderMeshBase>();
		newMesh.mRenderMeshs.push_back(meshDataPointer->mMeshData);
	}
	FGResourceView* colorView = node->AddRTV(shadowData->mLightShadowmap.get(), RHIViewDimension::TextureView2D);
	FGResourceView* depthView = node->AddDSV(shadowData->mLightShadowDepth.get());
	node->SetColorAttachment(colorView, LoadOp::kClear);
	node->SetDepthStencilAttachment(depthView);

	node->ExcuteFunc([view, renderScene](FrameGraphBuilder* builder, FGNode& node, RenderContext* device)
		{
			ShaderParamInputs shaderBindingParam;
			view->RequireData<RenderObjectDrawData>()->DrawRenderObjects(MeshRenderPass::DirectLightShadowDepthPass, shaderBindingParam, renderScene->mMainDirLight->mParamBuffer->mView.get());
		});
};

FGNode* DirectionalLightShadowPassGenerator::AddPassNode(FrameGraphBuilder* builder, RenderView* view, RenderScene* renderScene)
{
	if (!renderScene->mMainDirLight || !renderScene->mMainDirLight->mCastShadow)
		return;

}

void DirectionalLightShadowPass(FrameGraphBuilder* builder, RenderView* view, RenderScene* renderScene)
{
	if (!renderScene->mMainDirLight || !renderScene->mMainDirLight->mCastShadow)
		return;
	auto meshCmds = view->RequireData<RenderObjectDrawData>();
	meshCmds->SetROFilter(MeshRenderPass::DirectLightShadowDepthPass, [](RenderObject* renderObject)->bool
	{
		if (renderObject->mCastShadow)
		{
			return true;
		}
		return false;
	});
	static auto shadowMatAsset = sAssetModule->LoadAsset<MaterialTemplateAsset>("/assets/built-in/Depth.mat");
	static auto shadowMat = shadowMatAsset->GetDefaultInstance();
	if (shadowMat)
		shadowMat->Ready();
	meshCmds->SetOverrideMaterialInstance(MeshRenderPass::DirectLightShadowDepthPass, shadowMat);

	auto& node = builder->AddPass("Directional LightShadowmap");
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

	node.ExcuteFunc([view, renderScene](FrameGraphBuilder* builder, FGNode& node, RenderContext* device)
	{
		ShaderParamInputs shaderBindingParam;
		view->RequireData<RenderObjectDrawData>()->DrawRenderObjects(MeshRenderPass::DirectLightShadowDepthPass, shaderBindingParam, renderScene->mMainDirLight->mParamBuffer->mView.get());
	});
}

void PointShadowPass(FrameGraphBuilder* builder, RenderView* view, RenderScene* renderScene)
{
	if (renderScene->mPointLights.empty())
		return;

	bool hasShadow = false;
	for (PointLight* it : renderScene->mPointLights)
	{
		if (it->mCastShadow)
			hasShadow = true;
	}

	static auto shadowMatAsset = sAssetModule->LoadAsset<MaterialTemplateAsset>("/assets/built-in/Depth.mat");
	static auto shadowMat = shadowMatAsset->GetDefaultInstance();
	if (shadowMat)
		shadowMat->Ready();


	auto meshCmds = view->RequireData<RenderObjectDrawData>();
	meshCmds->SetROFilter(MeshRenderPass::PointLightShadowDepthPass, [](RenderObject* renderObject)->bool
	{
		if (renderObject->mCastShadow)
		{
			return true;
		}
		return false;
	});
	meshCmds->SetOverrideMaterialInstance(MeshRenderPass::PointLightShadowDepthPass, shadowMat);

	if (!hasShadow)
		return;
	for (uint32_t idx = 0; idx < 6; idx++)
	{
		auto& node = builder->AddPass(LString::Format("PointLightShadowmap{}", idx));

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

		node.ExcuteFunc([view, renderScene, idx](FrameGraphBuilder* builder, FGNode& node, RenderContext* device)
		{
			for (PointLight* it : renderScene->mPointLights)
			{
				if (!it->mCastShadow)
					continue;
				std::unordered_map<ShaderParamID, RHIView*> shaderBindingParam;
				view->RequireData<RenderObjectDrawData>()->DrawRenderObjects(MeshRenderPass::PointLightShadowDepthPass, shaderBindingParam, it->mParamBuffer[idx]->mView);
			}
		});
	}

}

}


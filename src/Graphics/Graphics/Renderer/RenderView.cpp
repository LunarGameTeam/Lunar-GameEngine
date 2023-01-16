#include "Graphics/Renderer/RenderView.h"
#include "Graphics/Renderer/RenderScene.h"
#include "Graphics/Asset/ShaderAsset.h"
#include "Graphics/RenderModule.h"
#include "Core/Asset/AssetModule.h"
#include "Graphics/Renderer/ScenePipeline.h"

namespace luna::render
{

RenderView::RenderView(uint64_t view_id) :
	mRT(nullptr),
	mViewID(view_id)
{

	RHIBufferDesc desc;
	desc.mBufferUsage = RHIBufferUsage::UniformBufferBit;
	desc.mSize = 1024;
	mPerViewBuffer = sRenderModule->GetRenderDevice()->CreateBuffer(desc);

	ViewDesc viewDesc;
	viewDesc.mViewType = RHIViewType::kConstantBuffer;
	viewDesc.mViewDimension = RHIViewDimension::BufferView;
	mPerViewBufferView = sRenderModule->GetRenderDevice()->CreateView(viewDesc);
	mPerViewBufferView->BindResource(mPerViewBuffer);

}

void RenderView::PrepareViewBuffer()
{
	static ShaderAsset* debugShader = sAssetModule->LoadAsset<render::ShaderAsset>("/assets/built-in/depth.hlsl");

	PerViewBuffer viewBuffer;	
	std::vector<byte> bufferPtr;
	auto passBufferDesc = debugShader->GetConstantBufferDesc("ViewBuffer");
	bufferPtr.resize(passBufferDesc.mBufferSize);
	{
		ConstantBufferVar& var = passBufferDesc.mVars["viewMatrix"];
		memcpy(bufferPtr.data() + var.mOffset, &mViewMatrix, var.mSize);
	}
	{
		ConstantBufferVar& var = passBufferDesc.mVars["projectionMatrix"];
		memcpy(bufferPtr.data() + var.mOffset, &mProjMatrix, var.mSize);
	}
	{
		LVector2f nearFar(mNear, mFar);
		ConstantBufferVar& var = passBufferDesc.mVars["nearFar"];
		memcpy(bufferPtr.data() + var.mOffset, &nearFar, var.mSize);
	}
	sRenderModule->GetRenderDevice()->UpdateConstantBuffer(mPerViewBuffer, bufferPtr.data(), bufferPtr.size());
}

void RenderView::GenerateSceneViewPass(RenderScene* scene, FrameGraphBuilder* FG)
{
	//mDebugPass->BuildRenderPass(FG, this, scene);	
}

void RenderView::GenerateShadowViewPass(RenderScene* scene, FrameGraphBuilder* FG)
{
}

void RenderView::ScenePipeline(RenderScene* scene, FrameGraphBuilder* FG)
{
	switch (mViewType)
	{
	case RenderViewType::SceneView:
	{
		BuildRenderPass(FG, this, scene);
		break;
	}	
	case RenderViewType::ShadowMapView:
	{
		break;
	}
	default:
		break;
	}


}

}
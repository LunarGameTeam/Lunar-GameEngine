#include "Graphics/Renderer/RenderView.h"

#include "Core/Asset/AssetModule.h"

#include "Graphics/RenderModule.h"
#include "Graphics/Asset/ShaderAsset.h"
#include "Graphics/Renderer/MaterialInstance.h"
#include "Graphics/Renderer/RenderLight.h"
#include "Graphics/Renderer/RenderScene.h"
#include "Graphics/Renderer/RenderContext.h"

#include "Graphics/Renderer/ScenePipeline.h"

namespace luna::graphics
{
PARAM_ID(ViewBuffer);
void RenderViewParameterData::SetMaterialViewParameter(MaterialInstance* matInstance)
{
	matInstance->SetShaderInput(ParamID_ViewBuffer, mViewParamCbufferView);
}

void RenderViewParameterData::Init()
{
	auto device = sRenderModule->GetRenderContext();
	mCbufferDesc = device->GetDefaultShaderConstantBufferDesc(LString("ViewBuffer").Hash());

	RHIBufferDesc desc;
	desc.mBufferUsage = RHIBufferUsage::UniformBufferBit;
	desc.mSize = SizeAligned2Pow(mCbufferDesc.mSize, CommonSize64K);
	ViewDesc viewDesc;
	viewDesc.mViewType = RHIViewType::kConstantBuffer;
	viewDesc.mViewDimension = RHIViewDimension::BufferView;
	mViewParamRes = sRenderModule->GetRenderContext()->CreateBuffer(RHIHeapType::Default, desc);
	mViewParamCbufferView = sRenderModule->GetRHIDevice()->CreateView(viewDesc);
	mViewParamCbufferView->BindResource(mViewParamRes);
}

RenderView::RenderView() :
	mRT(nullptr)
{	

}

void RenderView::Culling(RenderScene* scene)
{
	scene->GetRenderObjects(mViewVisibleROs);
}

}
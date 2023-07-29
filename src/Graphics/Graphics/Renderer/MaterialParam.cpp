#include "Graphics/Renderer/MaterialParam.h"

#include "Graphics/RenderModule.h"

#include "Graphics/Asset/MaterialTemplate.h"
#include "Graphics/Asset/TextureAsset.h"


#include "Graphics/RHI/RHIResource.h"

#include "Core/Memory/PtrBinding.h"
#include "Graphics/RHI/RHIPipeline.h"


namespace luna::graphics
{
//优化 增量更新
PARAM_ID(MaterialBuffer);

ShaderCBuffer::ShaderCBuffer(const RHICBufferDesc& cbDesc) :
	mVars(cbDesc.mVars)
{
	RHIBufferDesc desc;
	mData.resize(cbDesc.mSize);
	desc.mBufferUsage = RHIBufferUsage::UniformBufferBit;
	desc.mSize = cbDesc.mSize;
	ViewDesc viewDesc;
	viewDesc.mViewType = RHIViewType::kConstantBuffer;
	viewDesc.mViewDimension = RHIViewDimension::BufferView;
	mRes = sRenderModule->GetRenderContext()->CreateBuffer(desc);
	mView = sRenderModule->GetRHIDevice()->CreateView(viewDesc);
	mView->BindResource(mRes);
}

ShaderCBuffer::ShaderCBuffer(RHIBufferUsage usage, uint32_t size)
{
	RHIBufferDesc desc;
	mData.resize(size);
	desc.mBufferUsage = usage;
	desc.mSize = size;
	mRes = sRenderModule->GetRenderContext()->CreateBuffer(desc);
}

void ShaderCBuffer::Commit()
{
	sRenderModule->mRenderContext->UpdateConstantBuffer(mRes, mData.data(), mData.size() * sizeof(byte));
}

}
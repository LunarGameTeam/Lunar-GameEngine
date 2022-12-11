#include "render/renderer/render_device.h"
#include "render/renderer/render_obj.h"


#include "render/rhi/vulkan/vulkan_device.h"
#include "render/rhi/vulkan/vulkan_render_queue.h"

#include "render/rhi/DirectX12/dx12_device.h"
#include "render/rhi/DirectX12/dx12_render_queue.h"
#include "render/rhi/DirectX12/dx12_descriptor_impl.h"
#include "render/rhi/DirectX12/dx12_view.h"
#include "render/rhi/DirectX12/dx12_descriptor_pool.h"

#include "render/asset/mesh_asset.h"


namespace luna::render 
{

//上传堆最大容量，最大同时上传一个4096 * 4096的图
size_t sStagingBufferMaxSize = 4096 * 4096 * 64;
//FG堆最大容量
size_t sFrameGraphBufferMaxSize = 1024 * 1024 * 4 * 64;

size_t GetOffset(size_t offset, size_t aligment)
{
	return offset  + (aligment - offset) % aligment;
}

RENDER_API CONFIG_IMPLEMENT(LString, Render, RenderDeviceType, "DirectX12");

void RenderDevice::Init()
{
	if (Config_RenderDeviceType.GetValue() == "DirectX12")
		mDeviceType = RenderDeviceType::DirectX12;
	else
		mDeviceType = RenderDeviceType::Vulkan;

	switch (mDeviceType)
	{
	case luna::render::RenderDeviceType::DirectX12:
		mDevice = new DX12Device();		
		mDevice->InitDeviceData();
		mGraphicQueue = CreateRHIObject<DX12RenderQueue>();	
		mTransferQueue = CreateRHIObject<DX12RenderQueue>(RHIQueueType::eTransfer);
		break;
	case luna::render::RenderDeviceType::Vulkan:
		mDevice = new VulkanDevice();
		mDevice->InitDeviceData();
		mGraphicQueue = CreateRHIObject<VulkanRenderQueue>();
		mTransferQueue = CreateRHIObject<VulkanRenderQueue>(RHIQueueType::eTransfer);
		break;
	default:
		break;
	}
	mFence = mDevice->CreateFence();
	mGraphicCmd = mDevice->CreateCommondList(RHICmdListType::Graphic3D);
	mTransferCmd = mDevice->CreateCommondList(RHICmdListType::Copy);
	mBarrierCmd = mDevice->CreateCommondList(RHICmdListType::Graphic3D);

	//Default
	{
		SamplerDesc desc;
		desc.filter = SamplerFilter::kMinMagMipLinear;
		desc.func = SamplerComparisonFunc::kAlways;
		desc.mode = SamplerTextureAddressMode::kClamp;
		ViewDesc samplerDesc;
		samplerDesc.mViewType = RHIViewType::kSampler;

		mClampSampler = mDevice->CreateSamplerExt(desc);
		mClampSamplerView = mDevice->CreateView(samplerDesc);
		mClampSamplerView->BindResource(mClampSampler);

	}
	{
		DescriptorPoolDesc poolDesc;
		poolDesc.mPoolAllocateSizes[DescriptorHeapType::CBV_SRV_UAV] = 1024;
		poolDesc.mPoolAllocateSizes[DescriptorHeapType::SAMPLER] = 1024;
		poolDesc.mPoolAllocateSizes[DescriptorHeapType::DSV] = 100;
		poolDesc.mPoolAllocateSizes[DescriptorHeapType::RTV] = 100;
		mDefaultPool = mDevice->CreateDescriptorPool(poolDesc);

	}

	RHIMemoryDesc stagingMemoryDesc;
	stagingMemoryDesc.Type = RHIHeapType::Upload;
	stagingMemoryDesc.SizeInBytes = sStagingBufferMaxSize;
	mStagingMemory = mDevice->AllocMemory(stagingMemoryDesc, 31);

	//Frame Graph
	{
		RHIMemoryDesc FGMemoryDesc;
		FGMemoryDesc.Type = RHIHeapType::Default;
		FGMemoryDesc.SizeInBytes = sFrameGraphBufferMaxSize;
		mFGMemory = mDevice->AllocMemory(FGMemoryDesc, 3);
	}

	//记录一些初始化时的命令
	mGraphicCmd->Reset();
	mTransferCmd->Reset();
	mBarrierCmd->Reset();

}

void RenderDevice::OnFrameBegin()
{
	mFGOffset = 0;

	FlushStaging();
}

void RenderDevice::OnFrameEnd()
{
	
}

void RenderDevice::UpdateConstantBuffer(RHIResourcePtr target, void* data, size_t dataSize)
{
	void* dst = target->Map();
	memcpy(dst, data, dataSize);
	target->Unmap();
}

RHIResourcePtr RenderDevice::CreateFGTexture(const RHITextureDesc& textureDesc, const RHIResDesc& resDesc, void* initData /*= nullptr*/, size_t dataSize /*= 0*/)
{
	return _CreateTexture(textureDesc, resDesc, initData, dataSize, mFGMemory, mFGOffset);
}

RHIResourcePtr RenderDevice::CreateFGBuffer(const RHIBufferDesc& resDesc, void* initData)
{
	return _CreateBuffer(resDesc, initData, mFGMemory, mFGOffset);
}

RHIResourcePtr RenderDevice::_CreateBuffer(const RHIBufferDesc& desc, void* initData, RHIMemoryPtr targetMemory, size_t& memoryOffset)
{
	assert(desc.mSize != 0);
	RHIBufferDesc resDesc = desc;
	resDesc.mBufferUsage = resDesc.mBufferUsage | RHIBufferUsage::TransferDstBit;
	RHIResourcePtr dstBuffer = mDevice->CreateBufferExt(resDesc);

	RHIMemoryDesc memoryDesc;
	const MemoryRequirements& memoryReq = dstBuffer->GetMemoryRequirements();
	memoryDesc.SizeInBytes = memoryReq.size;
	memoryDesc.Type = RHIHeapType::Upload;

	if (targetMemory == nullptr)
	{
		targetMemory = mDevice->AllocMemory(memoryDesc, memoryReq.memory_type_bits);
		memoryOffset = 0;
	}
	else
	{
		memoryOffset = GetOffset(memoryOffset, memoryReq.alignment);
	}
	dstBuffer->BindMemory(targetMemory, memoryOffset);
	memoryOffset += memoryReq.size;

	if (initData != nullptr)
	{
		void* map_pointer = dstBuffer->Map();
		memcpy(map_pointer, initData, desc.mSize);
		dstBuffer->Unmap();
	}
	return dstBuffer;
}

RHIResourcePtr RenderDevice::CreateBuffer(const RHIBufferDesc& resDesc, void* initData)
{
	size_t offset = 0;
	return _CreateBuffer(resDesc, initData, nullptr, offset);
}

RHIResourcePtr RenderDevice::_CreateTexture(const RHITextureDesc& textureDesc, const RHIResDesc& resDesc, void* initData, size_t dataSize, RHIMemoryPtr targetMemory, size_t& memoryOffset)
{

	bool usingStaging = false;
	usingStaging = (initData != nullptr);

	RHIResourcePtr stagingBuffer = nullptr;

	RHIResDesc newResDesc = resDesc;
	newResDesc.mImageUsage = newResDesc.mImageUsage | RHIImageUsage::TransferDstBit;
	RHIResourcePtr textureRes = mDevice->CreateTextureExt(textureDesc, newResDesc);
	const MemoryRequirements& memoryReq = textureRes->GetMemoryRequirements();

	RHIMemoryDesc memoryDesc;
	memoryDesc.SizeInBytes = memoryReq.size;
	memoryDesc.Type = RHIHeapType::Default;
	if (targetMemory == nullptr)
	{
		targetMemory = mDevice->AllocMemory(memoryDesc, memoryReq.memory_type_bits);
		memoryOffset = 0;
	}
	else
	{
		memoryOffset = GetOffset(memoryOffset, memoryReq.alignment);
	}
	textureRes->BindMemory(targetMemory, memoryOffset);
	memoryOffset += memoryReq.size;

	if (initData != nullptr)
	{
		stagingBuffer = _AllocateStagingBuffer(initData, dataSize, textureRes);
	}


	ResourceBarrierDesc dstBarrier;
	dstBarrier.mBarrierRes = textureRes;
	dstBarrier.mBaseMipLevel = 0;
	dstBarrier.mMipLevels = resDesc.MipLevels;
	dstBarrier.mBaseDepth = 0;
	dstBarrier.mDepth = resDesc.DepthOrArraySize;
	if (usingStaging)
	{
		dstBarrier.mStateBefore = kUndefined;
		dstBarrier.mStateAfter = kCopyDest;
		mTransferCmd->ResourceBarrierExt(dstBarrier);
		mTransferCmd->CopyBufferToTexture(textureRes, 0, stagingBuffer, 0);
		if (Has(resDesc.mImageUsage, RHIImageUsage::SampledBit))
		{
			dstBarrier.mStateBefore = kCopyDest;
			dstBarrier.mStateAfter = kShaderReadOnly;
			mBarrierCmd->ResourceBarrierExt(dstBarrier);
		}
		else if (Has(resDesc.mImageUsage, RHIImageUsage::ColorAttachmentBit))
		{
			dstBarrier.mStateBefore = kCopyDest;
			dstBarrier.mStateAfter = kRenderTarget;
			mBarrierCmd->ResourceBarrierExt(dstBarrier);
		}		
	}


	return textureRes;
}

RHIResourcePtr RenderDevice::CreateTexture(const RHITextureDesc& textureDesc, const RHIResDesc& resDesc, void* initData, size_t dataSize)
{
	size_t offset = 0;
	return _CreateTexture(textureDesc, resDesc, initData, dataSize, nullptr, offset);
}

RHIViewPtr RenderDevice::CreateView(const ViewDesc& desc)
{	
	RHIViewPtr view = mDevice->CreateView(desc);
	return view;
}

PipelinePair RenderDevice::CreatePipelineState(const RHIPipelineStateDesc& desc)
{
	RHIPipelineStatePtr pipeline = mDevice->CreatePipeline(desc);
	RHIBindingSetPtr bindingSet = mDevice->CreateBindingSet(mDefaultPool, pipeline->GetBindingSetLayout());
	return std::make_pair(pipeline, bindingSet);
}

PipelinePair RenderDevice::CreatePipelineState(ShaderAsset* shaderAsset, const RenderPassDesc& passDesc, RHIVertexLayout* layout)
{
	size_t hashResult = 0;
	boost::hash_combine(hashResult, layout->Hash());
	boost::hash_combine(hashResult, passDesc.Hash());

	auto key = std::make_pair(shaderAsset, hashResult);
	auto it = mPipelineCache.find(key);
	if (it != mPipelineCache.end())
		return it->second;

	RHIPipelineStateDesc desc = {};
	desc.mType = RHICmdListType::Graphic3D;
	desc.mGraphicDesc.mInputLayout = *layout;
	desc.mGraphicDesc.mPipelineStateDesc.mVertexShader = shaderAsset->GetVertexShader();
	desc.mGraphicDesc.mPipelineStateDesc.mPixelShader = shaderAsset->GetPixelShader();
	desc.mGraphicDesc.mRenderPassDesc = mCurRenderPass;

	RHIBlendStateTargetDesc blend = {};
	desc.mGraphicDesc.mPipelineStateDesc.BlendState.RenderTarget.push_back(blend);
	mPipelineCache[key] = CreatePipelineState(desc);
	return mPipelineCache[key];
}

void RenderDevice::FlushStaging()
{
	mFence->Wait(mFenceValue);
	mTransferCmd->CloseCommondList();
	mTransferQueue->ExecuteCommandLists(mTransferCmd);
	mTransferQueue->Signal(mFence, ++mFenceValue);
	mFence->Wait(mFenceValue);
	mBarrierCmd->CloseCommondList();
	mGraphicQueue->ExecuteCommandLists(mBarrierCmd);
	mGraphicQueue->Signal(mFence, ++mFenceValue);
	mFence->Wait(mFenceValue);
	mTransferCmd->Reset();
	mBarrierCmd->Reset();
	mStagingOffset = 0;
	historyStagingBuffer.clear();
}

RHIResourcePtr RenderDevice::_AllocateStagingBuffer(void* initData, size_t dataSize, RHIResourcePtr resMemoryLayout)
{
	RHIBufferDesc stagingBufferDesc;
	stagingBufferDesc.mSize = resMemoryLayout->GetMemoryRequirements().size;
	stagingBufferDesc.mBufferUsage = RHIBufferUsage::TransferSrcBit;
	RHIResourcePtr stagingBuffer = mDevice->CreateBufferExt(stagingBufferDesc);
	const MemoryRequirements& reqDst = stagingBuffer->GetMemoryRequirements();
	mStagingOffset = GetOffset(mStagingOffset, reqDst.alignment);
	//超出StagingMemory大小，Flush一下
	if (mStagingOffset > sStagingBufferMaxSize)
	{
		FlushStaging();
		mStagingOffset = GetOffset(mStagingOffset, reqDst.alignment);
	}
	stagingBuffer->BindMemory(mStagingMemory, mStagingOffset);
	char* dst = (char*)stagingBuffer->Map();
	memcpy(dst, initData, dataSize);
	stagingBuffer->Unmap();
	mStagingOffset = mStagingOffset + reqDst.size;
	historyStagingBuffer.push_back(stagingBuffer);
	return stagingBuffer;
}

void RenderDevice::BeginRendering(const RenderPassDesc& desc)
{
	mGraphicCmd->BeginRender(desc);
	mCurRenderPass = desc;
}

void RenderDevice::EndRenderPass()
{
	mGraphicCmd->EndRender();	
}

void RenderDevice::DrawRenderOBject(render::RenderObject* ro, render::ShaderAsset* shader, PackedParams* params)
{
	RHIPipelineStatePtr pipeline;
	RHIBindingSetPtr bindingset;
	size_t hashResult = 0;
	boost::hash_combine(hashResult, mCurRenderPass.Hash());
	boost::hash_combine(hashResult, ro->mMesh->mVeretexLayout.Hash());
	auto key = std::make_pair(shader, hashResult);
	auto it = mPipelineCache.find(key);
	
	//Find cache
	if (it == mPipelineCache.end())
	{
		auto piplinePair = CreatePipelineState(shader, mCurRenderPass, &ro->mMesh->mVeretexLayout);
		bindingset = piplinePair.second;
		pipeline = piplinePair.first;
	}
	else
	{
		pipeline = it->second.first;
		bindingset = it->second.second;
	}

	mGraphicCmd->SetPipelineState(pipeline);
	//Pipeline Changed
	if (mLastPipline != pipeline.get())
	{
		std::vector<BindingDesc> desc;
		PARAM_ID(SampleTypeClamp);
		if (shader->HasBindPoint(ParamID_SampleTypeClamp))
			params->PushShaderParam(ParamID_SampleTypeClamp, sRenderModule->GetRenderDevice()->mClampSamplerView);

		for (auto& param : params->mParams)
		{
			if (shader->HasBindPoint(param.first))
			{
				desc.emplace_back(shader->GetBindPoint(param.first), param.second);
			}
		}
		bindingset->WriteBindings(desc);
		mLastPipline = pipeline;
	}
	mGraphicCmd->BindDesriptorSetExt(bindingset);

	if (pipeline == nullptr || bindingset == nullptr || shader==nullptr)
		return;

	auto* mesh = ro->mMesh;
	size_t vertexCount = mesh->mVertexData.size();

	RHIResource* vb = mesh->mVB;
	size_t indexCount = mesh->mIndexData.size();
	size_t indexStride = mesh->mIndexCount;
	RHIResource* ib = mesh->mIB;

	LVector<RHIVertexBufferDesc> descs;
	RHIVertexBufferDesc& vbDesc = descs.emplace_back();
	vbDesc.mOffset = 0;
	vbDesc.mBufferSize = vertexCount * mesh->mVeretexLayout.GetSize();
	vbDesc.mVertexLayout = &mesh->mVeretexLayout;
	vbDesc.mVertexRes = mesh->mVB;

	mGraphicCmd->SetVertexBuffer(descs, 0);
	mGraphicCmd->SetIndexBuffer(ib);
	mGraphicCmd->SetDrawPrimitiveTopology(
		RHIPrimitiveTopology::TriangleList);
	mGraphicCmd->DrawIndexedInstanced(indexCount, 1, 0, 0, ro->mID % 128);

}

}



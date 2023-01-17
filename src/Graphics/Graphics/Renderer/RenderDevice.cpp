#include "Graphics/Renderer/RenderDevice.h"

#include "Graphics/RenderModule.h"

#include "Graphics/RHI/Vulkan/VulkanDevice.h"
#include "Graphics/RHI/Vulkan/VulkanRenderQueue.h"

#include "Graphics/RHI/DirectX12/DX12Device.h"
#include "Graphics/RHI/DirectX12/DX12RenderQueue.h"

#include "Graphics/RHI/RHIFence.h"
#include "Graphics/RHI/RHICmdList.h"


#include "Graphics/Asset/MeshAsset.h"
#include "Graphics/Renderer/MaterialInstance.h"
#include "Graphics/Renderer/RenderScene.h"
#include "Graphics/Asset/MaterialTemplate.h"


namespace luna::render 
{

size_t sStagingBufferMaxSize = 4096 * 4096 * 64;
size_t sFrameGraphBufferMaxSize = 1024 * 1024 * 4 * 64;
static const size_t sInstancingBufferMaxSize = 1024 * 1024 * 16;

size_t GetOffset(size_t offset, size_t aligment)
{
	return offset  + (aligment - offset) % aligment;
}

void DynamicMemoryBuffer::Init(RHIDevice* device, const RHIHeapType memoryHeapType,const int32_t memoryType)
{
	mDevice = device;
	RHIMemoryDesc memoryDesc;
	memoryDesc.Type = memoryHeapType;
	memoryDesc.SizeInBytes = mMaxSize;
	mFullMemory = mDevice->AllocMemory(memoryDesc, memoryType);
}

RHIResource* DynamicMemoryBuffer::AllocateNewBuffer(void* initData, size_t dataSize, size_t bufferResSize)
{
	RHIBufferDesc newBufferDesc;
	newBufferDesc.mSize = bufferResSize;
	newBufferDesc.mBufferUsage = mBufferUsage;
	RHIResourcePtr newBuffer = mDevice->CreateBufferExt(newBufferDesc);
	const MemoryRequirements& reqDst = newBuffer->GetMemoryRequirements();
	mBufferOffset = GetOffset(mBufferOffset, reqDst.alignment);
	if (mBufferOffset > sStagingBufferMaxSize)
	{
		return nullptr;
	}
	newBuffer->BindMemory(mFullMemory, mBufferOffset);
	char* dst = (char*)newBuffer->Map();
	memcpy(dst, initData, dataSize);
	newBuffer->Unmap();
	mBufferOffset = mBufferOffset + reqDst.size;
	mhistoryBuffer.push_back(newBuffer);
	return newBuffer;
}

void DynamicMemoryBuffer::Reset()
{ 
	mBufferOffset = 0;
	mhistoryBuffer.clear();
};

RENDER_API CONFIG_IMPLEMENT(LString, Render, RenderDeviceType, "DirectX12");
RenderDevice::RenderDevice() : mStagingMemory(sStagingBufferMaxSize, RHIBufferUsage::TransferSrcBit), mInstancingIdMemory(sInstancingBufferMaxSize, RHIBufferUsage::VertexBufferBit)
{
};

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

	mStagingMemory.Init(mDevice, RHIHeapType::Upload, 31);
	mInstancingIdMemory.Init(mDevice, RHIHeapType::Upload, 31);

	//Frame Graph
	{
		RHIMemoryDesc FGMemoryDesc;
		FGMemoryDesc.Type = RHIHeapType::Default;
		FGMemoryDesc.SizeInBytes = sFrameGraphBufferMaxSize;
		mFGMemory = mDevice->AllocMemory(FGMemoryDesc, 3);
	}

	mGraphicCmd->Reset();
	mTransferCmd->Reset();
	mBarrierCmd->Reset();

}

void RenderDevice::OnFrameBegin()
{
	mFGOffset = 0;

	FlushStaging();
	FlushFrameInstancingBuffer();
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

RHIResourcePtr RenderDevice::FGCreateTexture(const RHITextureDesc& textureDesc, const RHIResDesc& resDesc, void* initData /*= nullptr*/, size_t dataSize /*= 0*/)
{
	return _CreateTexture(textureDesc, resDesc, initData, dataSize, mFGMemory, mFGOffset);
}

RHIResourcePtr RenderDevice::FGCreateBuffer(const RHIBufferDesc& resDesc, void* initData)
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
		size_t fullResSize = textureRes->GetMemoryRequirements().size;
		stagingBuffer = mStagingMemory.AllocateNewBuffer(initData, dataSize, fullResSize);
		if (stagingBuffer == nullptr)
		{
			FlushStaging();
			stagingBuffer = mStagingMemory.AllocateNewBuffer(initData, dataSize, fullResSize);
		}
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

RHIResource* RenderDevice::CreateInstancingBufferByRenderObjects(LArray<RenderObject*>& RenderObjects)
{
	std::vector<int32_t> all_object_id;
	for (auto it : RenderObjects)
	{
		RenderObject* ro = it;
		all_object_id.push_back(ro->mID);
		all_object_id.push_back(0);
		all_object_id.push_back(0);
		all_object_id.push_back(0);
	}
	return mInstancingIdMemory.AllocateNewBuffer(all_object_id.data(), all_object_id.size() * sizeof(int32_t), all_object_id.size() * sizeof(int32_t));
}

RHIPipelineStatePtr RenderDevice::CreatePipelineState(MaterialInstance* mat, const RenderPassDesc& passDesc, RHIVertexLayout* layout)
{
	size_t hashResult = 0;
	boost::hash_combine(hashResult, layout->Hash());
	boost::hash_combine(hashResult, passDesc.Hash());

	auto key = std::make_pair(mat, hashResult);
	auto it = mPipelineCache.find(key);
	if (it != mPipelineCache.end())
		return it->second;

	RHIPipelineStateDesc desc = {};
	RenderPipelineStateDescGraphic& graphicDesc = desc.mGraphicDesc;
	desc.mType = RHICmdListType::Graphic3D;

	graphicDesc.mPipelineStateDesc.DepthStencilState.DepthEnable = mat->mMaterialTemplate->IsDepthTestEnable();
	graphicDesc.mPipelineStateDesc.DepthStencilState.DepthWrite = mat->mMaterialTemplate->IsDepthTestEnable();
	graphicDesc.mInputLayout = *layout;
	graphicDesc.mPipelineStateDesc.mVertexShader = mat->GetShaderVS();
	graphicDesc.mPipelineStateDesc.mPixelShader = mat->GetShaderPS();

	graphicDesc.mRenderPassDesc = mCurRenderPass;

	RHIBlendStateTargetDesc blend = {};
	desc.mGraphicDesc.mPipelineStateDesc.BlendState.RenderTarget.push_back(blend);
	auto pipeline = mDevice->CreatePipeline(desc);
	return pipeline;
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
	mStagingMemory.Reset();
}

void RenderDevice::BeginRenderPass(const RenderPassDesc& desc)
{
	mGraphicCmd->BeginRender(desc);
	mCurRenderPass = desc;
}

void RenderDevice::EndRenderPass()
{
	mGraphicCmd->EndRender();	
}

RenderDevice::PipelineCacheKey RenderDevice::PreparePipeline(render::MaterialInstance* mat, render::SubMesh* mesh, PackedParams* params)
{
// 
// 	RHIPipelineStatePtr pipeline;
// 	RHIBindingSetPtr bindingset;
// 	size_t hashResult = 0;
// 	boost::hash_combine(hashResult, mCurRenderPass.Hash());
// 	boost::hash_combine(hashResult, mesh->mVeretexLayout.Hash());
// 	auto key = std::make_pair(mat, hashResult);
// 	auto it = mPipelineCache.find(key);
// 
// 	//Find cache
// 	if (it == mPipelineCache.end() || it->second.first == nullptr || it->second.second == nullptr)
// 	{
// 		auto piplinePair = CreatePipelineState(mat, mCurRenderPass, &mesh->mVeretexLayout);
// 		bindingset = piplinePair.second;
// 		pipeline = piplinePair.first;
// 	}
// 	else
// 	{
// 		pipeline = it->second.first;
// 		bindingset = it->second.second;
// 	}
// 	ShaderAsset* shader = mat->GetShaderAsset();
// 	mGraphicCmd->SetPipelineState(pipeline);
// 	//Pipeline Changed
// 	if (mLastPipline != pipeline.get())
// 	{
// 		std::vector<BindingDesc> desc;
// 		PARAM_ID(SampleTypeClamp);
// 		if (shader->HasBindPoint(ParamID_SampleTypeClamp))
// 			params->PushShaderParam(ParamID_SampleTypeClamp, sRenderModule->GetRenderDevice()->mClampSamplerView);
// 
// 		for (auto& param : params->mParams)
// 		{
// 			if (shader->HasBindPoint(param.first))
// 			{
// 				desc.emplace_back(shader->GetBindPoint(param.first), param.second);
// 			}
// 		}
// 		bindingset->WriteBindings(desc);
// 		mLastPipline = pipeline;
// 	}
// 	mGraphicCmd->BindDesriptorSetExt(bindingset);
	return PipelineCacheKey();
}

void RenderDevice::DrawRenderOBject(render::RenderObject* ro, render::MaterialInstance* mat, PackedParams* params, render::RHIResource* instanceMessage, int32_t instancingSize)
{
	auto key = PreparePipeline(mat, ro->mMesh, params);
	DrawMesh(ro->mMesh, mat, params, instanceMessage, instancingSize);
}

void RenderDevice::DrawMesh(render::SubMesh* mesh, render::MaterialInstance* mat, PackedParams* params, render::RHIResource* instanceMessage, int32_t instancingSize)
{
// 	auto key = PreparePipeline(mat, mesh, params);
// 	RHIPipelineStatePtr pipeline = mPipelineCache[key].first;
// 
// 	size_t vertexCount = mesh->mVertexData.size();
// 
// 	RHIResource* vb = mesh->mVB;
// 	size_t indexCount = mesh->mIndexData.size();
// 	size_t indexStride = mesh->mIndexCount;
// 	RHIResource* ib = mesh->mIB;
// 
// 	LArray<RHIVertexBufferDesc> descs;
// 	RHIVertexBufferDesc& vbDesc = descs.emplace_back();
// 	vbDesc.mOffset = 0;
// 	vbDesc.mBufferSize = vertexCount * mesh->GetStridePerVertex();
// 	vbDesc.mVertexStride = mesh->GetStridePerVertex();
// 	vbDesc.mVertexRes = mesh->mVB;
// 	if (instanceMessage != nullptr)
// 	{
// 		RHIVertexBufferDesc& vbInstancingDesc = descs.emplace_back();
// 		vbInstancingDesc.mOffset = 0;
// 		vbInstancingDesc.mBufferSize = instancingSize * mesh->GetStridePerInstance();
// 		vbInstancingDesc.mVertexStride = mesh->GetStridePerInstance();
// 		vbInstancingDesc.mVertexRes = instanceMessage;
// 	}
// 	mGraphicCmd->SetVertexBuffer(descs, 0);
// 
// 
// 
// 	mGraphicCmd->SetIndexBuffer(ib);
// 	mGraphicCmd->SetDrawPrimitiveTopology(
// 		RHIPrimitiveTopology::TriangleList);
// 	mGraphicCmd->DrawIndexedInstanced((uint32_t)indexCount, 1, 0, 0, 0);

}

}



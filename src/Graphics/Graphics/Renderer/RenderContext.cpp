#include "Graphics/Renderer/RenderContext.h"

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
#include "Core/Asset/AssetModule.h"
#include "Graphics/Renderer/RenderView.h"

namespace luna::graphics 
{

size_t RhiObjectCache::GetDataGlobelId(const RHIObject* customData)
{
	auto dataExist = mDataGlobelId.find(customData);
	if (dataExist != mDataGlobelId.end())
	{
		return dataExist->second;
	}
	return size_t(-1);
}

void RhiObjectCache::ReleaseData(LMemoryHash& newHash)
{
	auto dataExist = mData.find(newHash);
	if (dataExist != mData.end())
	{
		mData.erase(dataExist);
		emptyID.push(mDataGlobelId[dataExist->second]);
		mDataGlobelId.erase(dataExist->second);
	}
}

RHIShaderBlobPtr ShaderBlobCache::CreateShader(RenderContext* mDevice, const RHIShaderDesc& desc)
{
	Log("Graphics", "Create shader:{}", desc.mName);
	
	std::function<void(luna::LMemoryHash&, const luna::graphics::RHIShaderDesc&)> dataHashFunc = [&](luna::LMemoryHash& newHash, const luna::graphics::RHIShaderDesc& desc)
	{
		newHash.Combine((uint8_t*)desc.mName.c_str(), desc.mName.Length() * sizeof(char));
		//newHash.Combine((uint8_t*)desc.mContent.c_str(), desc.mContent.Length() * sizeof(char));
		newHash.Combine((uint8_t*)desc.mEntryPoint.c_str(), desc.mEntryPoint.Length() * sizeof(char));
		newHash.Combine((uint8_t*)&desc.mType, sizeof(desc.mType));
		LArray<RhiShaderMacro> curMacroValues = desc.mShaderMacros;
		std::sort(curMacroValues.begin(), curMacroValues.end(), [&](const RhiShaderMacro& a, const RhiShaderMacro& b)->bool {
			return a.mMacroName.Compare(b.mMacroName) > 0;
			}
		);
		for (RhiShaderMacro& eachMacro : curMacroValues)
		{
			newHash.Combine((uint8_t*)eachMacro.mMacroName.c_str(), eachMacro.mMacroName.Length() * sizeof(char));
			char divide = '\t';
			newHash.Combine((uint8_t*)&divide, sizeof(divide));
			newHash.Combine((uint8_t*)eachMacro.mMacroValue.c_str(), eachMacro.mMacroValue.Length() * sizeof(char));
		}
		newHash.GenerateHash();
	};
	std::function<RHIShaderBlobPtr(RenderContext* device, const RHIShaderDesc &desc)> dataCreateFunc = [&](RenderContext* device, const RHIShaderDesc& desc)->RHIShaderBlobPtr
	{
		return device->mDevice->CreateShader(desc);
	};
	return CreateRHIObject<RHIShaderBlob,const luna::graphics::RHIShaderDesc&>(
		mDevice,
		desc,
		dataHashFunc,
		dataCreateFunc
	);
}

void PipelineStateCache::PackShaderToMemory(RenderContext* mDevice, luna::LMemoryHash& newHash, const RHIShaderBlob* shaderData)
{
	size_t shader_id = -1;
	if (shaderData != nullptr)
	{
		shader_id = mDevice->GetShaderId(shaderData);
		
	}
	newHash.Combine((uint8_t*)&shader_id, sizeof(size_t));
}

RHIPipelineStatePtr PipelineStateCache::CreatePipelineGraphic(
	RenderContext* mDevice,
	const RHIPipelineStateGraphDrawDesc& desc,
	const RHIVertexLayout& inputLayout,
	const RenderPassDesc& renderPassDesc
)
{
	std::function<void(luna::LMemoryHash&, const RHIPipelineStateGraphDrawDesc&,
		const RHIVertexLayout&,
		const RenderPassDesc&)> dataHashFunc = [&](
			luna::LMemoryHash& newHash,
			const RHIPipelineStateGraphDrawDesc& desc,
			const RHIVertexLayout& inputLayout,
			const RenderPassDesc& renderPassDesc
			)
	{
		//graph desc shader信息
		for (auto& eachShader : desc.mShaders)
		{
			PackShaderToMemory(mDevice, newHash, eachShader.second.get());
		}
		//input element信息
		for (auto& element : inputLayout.mElements)
		{
			newHash.Combine((uint8_t*)&element.mElementType, sizeof(element.mElementType));
			newHash.Combine((uint8_t*)&element.mElementCount, sizeof(element.mElementCount));
			newHash.Combine((uint8_t*)&element.mUsage, sizeof(element.mUsage));
			newHash.Combine((uint8_t*)&element.mOffset, sizeof(element.mOffset));
			newHash.Combine((uint8_t*)&element.mBufferSlot, sizeof(element.mBufferSlot));
			newHash.Combine((uint8_t*)&element.mInstanceUsage, sizeof(element.mInstanceUsage));
		}
		//pass信息
		for (auto& view : renderPassDesc.mColorView)
		{
			const RHITextureFormat& srv_desc = view->mBindResource->GetDesc().Format;
			newHash.Combine((const uint8_t*)&srv_desc, sizeof(srv_desc));
		}
		if (renderPassDesc.mDepths.size() > 0)
		{
			const RHITextureFormat& dsv_desc = renderPassDesc.mDepthStencilView->mBindResource->GetDesc().Format;
			newHash.Combine((const uint8_t*)&dsv_desc, sizeof(dsv_desc));
		}
		else
		{
			RHITextureFormat dsv_desc = RHITextureFormat::FORMAT_UNKNOWN;
			newHash.Combine((uint8_t*)&dsv_desc, sizeof(dsv_desc));
		}
		//graph desc state信息
		newHash.Combine((uint8_t*)&desc.BlendState.AlphaToCoverageEnable, sizeof(desc.BlendState.AlphaToCoverageEnable));
		newHash.Combine((uint8_t*)&desc.BlendState.IndependentBlendEnable, sizeof(desc.BlendState.IndependentBlendEnable));
		newHash.Combine((uint8_t*)desc.BlendState.RenderTarget.data(), desc.BlendState.RenderTarget.size() * sizeof(RHIBlendStateTargetDesc));
		newHash.Combine((uint8_t*)&desc.SampleMask, sizeof(desc.SampleMask));
		newHash.Combine((uint8_t*)&desc.RasterizerState, sizeof(desc.RasterizerState));
		newHash.Combine((uint8_t*)&desc.DepthStencilState, sizeof(desc.DepthStencilState));
		newHash.Combine((uint8_t*)&desc.IBStripCutValue, sizeof(desc.IBStripCutValue));
		newHash.Combine((uint8_t*)&desc.PrimitiveTopologyType, sizeof(desc.PrimitiveTopologyType));
		newHash.Combine((uint8_t*)&desc.NumRenderTargets, sizeof(desc.NumRenderTargets));
		newHash.Combine((uint8_t*)&desc.SampleDesc, sizeof(desc.SampleDesc));
		newHash.Combine((uint8_t*)&desc.NodeMask, sizeof(desc.NodeMask));
		newHash.GenerateHash();
	};

	std::function<RHIPipelineStatePtr(
		RenderContext* device, 
		const RHIPipelineStateGraphDrawDesc& desc,
		const RHIVertexLayout& inputLayout,
		const RenderPassDesc& renderPassDesc)> dataCreateFunc = [&](
			RenderContext* device, 
			const RHIPipelineStateGraphDrawDesc& desc,
			const RHIVertexLayout& inputLayout,
			const RenderPassDesc& renderPassDesc)->RHIPipelineStatePtr
	{
		auto vsItor = desc.mShaders.find(RHIShaderType::Vertex);
		if (vsItor != desc.mShaders.end())
		{
			Log("Graphics", "Create pipeline for shader:{}", vsItor->second->mDesc.mName);
		}
		return device->mDevice->CreatePipelineGraphic(desc, inputLayout, renderPassDesc);
	};

	return CreateRHIObject<RHIPipelineState,
		const RHIPipelineStateGraphDrawDesc&,
		const RHIVertexLayout&,
		const RenderPassDesc&>(
		mDevice,
		desc,
		inputLayout,
		renderPassDesc,
		dataHashFunc,
		dataCreateFunc
	);
}

RHIPipelineStatePtr PipelineStateCache::CreatePipelineCompute(
	RenderContext* mDevice,
	const RHIPipelineStateComputeDesc& desc
)
{
	std::function<void(luna::LMemoryHash&, const luna::graphics::RHIPipelineStateComputeDesc&)> dataHashFunc = [&](luna::LMemoryHash& newHash, const luna::graphics::RHIPipelineStateComputeDesc& desc)
	{	
		for (auto& eachShader : desc.mShaders)
		{
			PackShaderToMemory(mDevice, newHash, eachShader.second.get());
		}
		newHash.GenerateHash();
	};
	std::function<RHIPipelineStatePtr(RenderContext* device, const RHIPipelineStateComputeDesc& desc)> dataCreateFunc = [&](RenderContext* device, const RHIPipelineStateComputeDesc& desc)->RHIPipelineStatePtr
	{
		auto vsItor = desc.mShaders.find(RHIShaderType::Compute);
		if (vsItor != desc.mShaders.end())
		{
			Log("Graphics", "Create pipeline for shader:{}", vsItor->second->mDesc.mName);
		}
		return device->mDevice->CreatePipelineCompute(desc);
	};
	return CreateRHIObject<RHIPipelineState, const luna::graphics::RHIPipelineStateComputeDesc&>(
		mDevice,
		desc,
		dataHashFunc,
		dataCreateFunc
	);
}

RHICmdSignaturePtr CmdSignatureCache::CreateCmdSignature(
	RenderContext* mDevice,
	RHIPipelineState* pipeline,
	const LArray<CommandArgDesc>& allCommondDesc
)
{
	std::function<void(luna::LMemoryHash&, RHIPipelineState* pipeline, const LArray<CommandArgDesc>& allCommondDesc)> dataHashFunc = [&](luna::LMemoryHash& newHash, RHIPipelineState* pipeline, const LArray<CommandArgDesc>& allCommondDesc)
	{
		size_t pipelineId = mDevice->GetPipelineId(pipeline);
		newHash.Combine((uint8_t*)&pipelineId, sizeof(size_t));
		newHash.Combine((uint8_t*)allCommondDesc.data(), allCommondDesc.size() * sizeof(CommandArgDesc));
		newHash.GenerateHash();
	};
	std::function<RHICmdSignaturePtr(RenderContext* device, RHIPipelineState* pipeline,const LArray<CommandArgDesc>& allCommondDesc)> dataCreateFunc = [&](
		RenderContext* device,RHIPipelineState* pipeline,const LArray<CommandArgDesc>& allCommondDesc)->RHICmdSignaturePtr
	{
		return device->mDevice->CreateCmdSignature(pipeline, allCommondDesc);
	};
	return CreateRHIObject<RHICmdSignature, RHIPipelineState*, const LArray<CommandArgDesc>&>(
		mDevice,
		pipeline,
		allCommondDesc,
		dataHashFunc,
		dataCreateFunc
		);
}

const size_t sStagingBufferMaxSize = 1024 * 1024 * 32 * 8;
const size_t sFrameGraphBufferMaxSize = 1024 * 1024 * 32 * 8;
const size_t sInstancingBufferMaxSize = 128 * 128;

size_t GetOffset(size_t offset, size_t aligment)
{
	return offset  + (aligment - offset) % aligment;
}

void StaticSampler::Init(SamplerDesc& desc, ViewDesc& view)
{
	mSampler = sRenderModule->GetRHIDevice()->CreateSamplerExt(desc);
	mView = sRenderModule->GetRHIDevice()->CreateView(view);
	mView->BindResource(mSampler);
};

RenderContext::RenderContext()
{
	mFullScreenRenderMesh = luna::sRenderModule->GetAssetManager()->GetFullScreenMesh();
};

void RenderContext::Init()
{
	if (Config_RenderDeviceType.GetValue() == "DirectX12")
		mDeviceType = RenderDeviceType::DirectX12;
	else
		mDeviceType = RenderDeviceType::Vulkan;
	mDevice = GenerateRenderDevice();
	mDevice->InitDeviceData();
	mGraphicQueue = GenerateRenderQueue();
	mTransferQueue = GenerateRenderQueue(RHIQueueType::eTransfer);
	mFence = mDevice->CreateFence();

	mGraphicCmd = mDevice->CreateSinglePoolSingleCommondList(RHICmdListType::Graphic3D);
	mTransferCmd = mDevice->CreateSinglePoolSingleCommondList(RHICmdListType::Copy);
	mBarrierCmd = mDevice->CreateSinglePoolSingleCommondList(RHICmdListType::Graphic3D);

	mStagingBufferPool = std::make_shared<RHIStagingBufferPool>(mDevice);
	//Default
	{
		SamplerDesc desc;
		desc.filter = SamplerFilter::kMinMagMipLinear;
		desc.func = SamplerComparisonFunc::kNever;
		desc.mode = SamplerTextureAddressMode::kClamp;
		ViewDesc samplerDesc;
		samplerDesc.mViewType = RHIViewType::kSampler;
		mClamp.Init(desc, samplerDesc);

	}
	{
		SamplerDesc desc;		
		desc.filter = SamplerFilter::kAnisotropic;
		desc.func = SamplerComparisonFunc::kNever;
		desc.mode = SamplerTextureAddressMode::kWrap;
		ViewDesc samplerDesc;
		samplerDesc.mViewType = RHIViewType::kSampler;
		mRepeat.Init(desc, samplerDesc);
	}
	{
		DescriptorPoolDesc poolDesc;
		poolDesc.mPoolAllocateSizes[DescriptorHeapType::CBV_SRV_UAV] = 7192;
		poolDesc.mPoolAllocateSizes[DescriptorHeapType::SAMPLER] = 1024;
		poolDesc.mPoolAllocateSizes[DescriptorHeapType::DSV] = 100;
		poolDesc.mPoolAllocateSizes[DescriptorHeapType::RTV] = 100;
		mDefaultPool = mDevice->CreateDescriptorPool(poolDesc);

	}

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
	
	
	LArray<ShaderMacro*> emptyShaderMacros;
	mDefaultShader = sAssetModule->LoadAsset<ShaderAsset>("/assets/built-in/Shader/Debug.hlsl");
	
	mDefaultShaderVertexInstance = mDefaultShader->GenerateShaderInstance(RHIShaderType::Vertex, emptyShaderMacros);
	mDefaultShaderFragmentInstance = mDefaultShader->GenerateShaderInstance(RHIShaderType::Pixel, emptyShaderMacros);
	mDefaultShaderPbr = sAssetModule->LoadAsset<ShaderAsset>("/assets/built-in/Shader/Pbr.hlsl");
	mDefaultShaderVertexPbrInstance = mDefaultShaderPbr->GenerateShaderInstance(RHIShaderType::Vertex, emptyShaderMacros);
	mDefaultShaderFragmentPbrInstance = mDefaultShaderPbr->GenerateShaderInstance(RHIShaderType::Pixel, emptyShaderMacros);
	PARAM_ID(ViewBuffer);
	PARAM_ID(SceneBuffer);

	RHIBindPoint bindpoint;
	if (mDefaultShaderVertexInstance->GetRhiShader()->HasBindPoint(ParamID_ViewBuffer))
	{
		bindpoint = mDefaultShaderVertexInstance->GetRhiShader()->GetBindPoint(ParamID_ViewBuffer)->second;
	}
	else 
	{
		bindpoint = mDefaultShaderFragmentInstance->GetRhiShader()->GetBindPoint(ParamID_ViewBuffer)->second;
	}
	

	LArray<RHIBindPoint> bindingpoints;
	bindingpoints.push_back(bindpoint);
	mViewBindingSet = mDevice->CreateBindingSetLayout(bindingpoints);


	RHIBufferDesc desc;
	emptyInstanceBufferSize = sizeof(uint32_t) * 4 * 128;
	desc.mBufferUsage = RHIBufferUsage::VertexBufferBit;
	desc.mSize = emptyInstanceBufferSize;
	emptyInstanceBuffer = CreateBuffer(RHIHeapType::Upload, desc);

	RHIBufferDesc instancingDesc;
	instancingDesc.mBufferUsage = RHIBufferUsage::VertexBufferBit;
	instancingDesc.mSize = sizeof(uint32_t) * 4 * 2048;
	renderObjectInstancingBuffer = CreateBuffer(RHIHeapType::Upload,instancingDesc);
}

void RenderContext::OnFrameBegin()
{
	ZoneScoped;
	mFGOffset = 0;

	FlushStaging();
	mStagingBufferPool->TickPoolRefresh();
}

void RenderContext::OnFrameEnd()
{
	mBindingSetCache.clear();
}

void RenderContext::UpdateConstantBuffer(RHIResourcePtr target, void* data, size_t dataSize)
{
	void* dst = target->Map();
	memcpy(dst, data, dataSize);
	target->Unmap();
}

RHIResourcePtr RenderContext::FGCreateTexture(const RHIResDesc& resDesc)
{
	RHIResourcePtr res = _CreateTextureByMemory(resDesc, mFGMemory, mFGOffset);
	const MemoryRequirements& memoryReq = res->GetMemoryRequirements();
	mFGOffset += memoryReq.size;
	return res;
}

RHIResourcePtr RenderContext::FGCreateBuffer(const RHIBufferDesc& resDesc)
{
	RHIResourcePtr res = _CreateBufferByMemory(resDesc, mFGMemory, mFGOffset);
	const MemoryRequirements& memoryReq = res->GetMemoryRequirements();
	mFGOffset += memoryReq.size;
	return res;
}

RHIResourcePtr RenderContext::_CreateBufferByMemory(const RHIBufferDesc& desc, RHIMemoryPtr targetMemory, size_t& memoryOffset)
{
	assert(desc.mSize != 0);
	RHIBufferDesc resDesc = desc;
	resDesc.mBufferUsage = resDesc.mBufferUsage | RHIBufferUsage::TransferDstBit;
	RHIResourcePtr dstBuffer = mDevice->CreateBufferExt(resDesc);
	dstBuffer->BindMemory(targetMemory, memoryOffset);
	return dstBuffer;
}

RHIResourcePtr RenderContext::_CreateTextureByMemory(const RHIResDesc& resDesc, RHIMemoryPtr targetMemory, size_t& memoryOffset)
{
	RHIResDesc newResDesc = resDesc;
	newResDesc.mImageUsage = newResDesc.mImageUsage | RHIImageUsage::TransferDstBit;
	RHIResourcePtr textureRes = mDevice->CreateTextureExt(newResDesc);
	const MemoryRequirements& memoryReq = textureRes->GetMemoryRequirements();
	textureRes->BindMemory(targetMemory, memoryOffset);
	
	return textureRes;
}

RHIResourcePtr RenderContext::_CreateBuffer(RHIHeapType memoryType, const RHIBufferDesc& desc, void* initData,size_t initDataSize)
{
	assert(desc.mSize != 0);
	RHIBufferDesc resDesc = desc;
	resDesc.mBufferUsage = resDesc.mBufferUsage | RHIBufferUsage::TransferDstBit;
	RHIResourcePtr dstBuffer = mDevice->CreateBufferExt(resDesc);
	dstBuffer->BindMemory(memoryType);
	if (initData == nullptr)
	{
		return dstBuffer;
	}
	assert(initDataSize != 0);
	if (memoryType == RHIHeapType::Upload)
	{
		void* dataPointer = dstBuffer->Map();
		memcpy(dataPointer, initData, resDesc.mSize);
		dstBuffer->Unmap();
	}
	else
	{
		mStagingBufferPool->UploadToDstBuffer(desc.mSize, initData, dstBuffer, 0);
	}
	return dstBuffer;
}

RHIResourcePtr RenderContext::CreateBuffer(RHIHeapType memoryType, const RHIBufferDesc& resDesc, void* initData, size_t initDataSize)
{
	size_t offset = 0;
	return _CreateBuffer(memoryType,resDesc, initData, initDataSize);
}

RHIResourcePtr RenderContext::_CreateTexture(const RHIResDesc& resDesc, void* initData, size_t dataSize)
{

	bool usingStaging = false;
	usingStaging = (initData != nullptr);

	RHIResourcePtr stagingBuffer = nullptr;

	RHIResDesc newResDesc = resDesc;
	newResDesc.mImageUsage = newResDesc.mImageUsage | RHIImageUsage::TransferDstBit;
	RHIResourcePtr textureRes = mDevice->CreateTextureExt(newResDesc);
	const MemoryRequirements& memoryReq = textureRes->GetMemoryRequirements();
	textureRes->BindMemory(RHIHeapType::Default);
	if (initData == nullptr) 
	{
		return textureRes;
	}
	assert(dataSize != 0);
	mStagingBufferPool->UploadToDstTexture(dataSize, initData, textureRes, 0);

	ResourceBarrierDesc dstBarrier;
	dstBarrier.mBarrierRes = textureRes;
	dstBarrier.mBaseMipLevel = 0;
	dstBarrier.mMipLevels = resDesc.MipLevels;
	dstBarrier.mBaseDepth = 0;
	dstBarrier.mDepth = resDesc.DepthOrArraySize;
	if (Has(resDesc.mImageUsage, RHIImageUsage::SampledBit))
	{
		dstBarrier.mStateBefore = kCommon;
		dstBarrier.mStateAfter = kShaderReadOnly;
		mBarrierCmd->GetCmdList()->ResourceBarrierExt(dstBarrier);
	}
	else if (Has(resDesc.mImageUsage, RHIImageUsage::ColorAttachmentBit))
	{
		dstBarrier.mStateBefore = kCopyDest;
		dstBarrier.mStateAfter = kRenderTarget;
		mBarrierCmd->GetCmdList()->ResourceBarrierExt(dstBarrier);
	}
	return textureRes;
}

RHIResourcePtr RenderContext::CreateTexture(const RHIResDesc& resDesc, void* initData, size_t dataSize)
{
	size_t offset = 0;
	return _CreateTexture(resDesc, initData, dataSize);
}

RHIResource* RenderContext::CreateInstancingBufferByRenderObjects(const LArray<RenderObject*>& RenderObjects)
{
	std::vector<int32_t> ids;
	for (auto it : RenderObjects)
	{
		RenderObject* ro = it;
		ids.push_back(ro->mID);
		ids.push_back(0);
		ids.push_back(0);
		ids.push_back(0);
	}
	void* pointer = renderObjectInstancingBuffer->Map();
	memcpy(pointer, ids.data(), ids.size() * sizeof(int32_t));
	renderObjectInstancingBuffer->Unmap();
	return renderObjectInstancingBuffer;
}

RHIShaderBlobPtr RenderContext::CreateShader(const RHIShaderDesc& desc)
{
	return mShaderBlobCache.CreateShader(this,desc);
}

size_t RenderContext::GetShaderId(const RHIShaderBlob* shader)
{
	return mShaderBlobCache.GetDataGlobelId(shader);
}

RHIPipelineStatePtr RenderContext::CreatePipelineGraphic(
	const RHIPipelineStateGraphDrawDesc& desc,
	const RHIVertexLayout& inputLayout,
	const RenderPassDesc& renderPassDesc
)
{
	return mPipelineCache.CreatePipelineGraphic(this, desc, inputLayout, renderPassDesc);
}

RHIPipelineStatePtr RenderContext::CreatePipelineCompute(const RHIPipelineStateComputeDesc& desc)
{
	return mPipelineCache.CreatePipelineCompute(this, desc);
}

size_t RenderContext::GetPipelineId(const RHIPipelineState* pipeline)
{
	return mPipelineCache.GetDataGlobelId(pipeline);
}

RHICmdSignaturePtr RenderContext::CreateCmdSignature(RHIPipelineState* pipeline, const LArray<CommandArgDesc>& allCommondDesc)
{
	return mCmdSignatureCache.CreateCmdSignature(this, pipeline, allCommondDesc);
}

size_t RenderContext::GetCmdSignatureId(const RHICmdSignature* pipeline)
{
	return mCmdSignatureCache.GetDataGlobelId(pipeline);
}

RHICBufferDesc RenderContext::GetDefaultShaderConstantBufferDesc(ShaderParamID name)
{
	if (mDefaultShaderVertexPbrInstance->GetRhiShader()->HasUniformBuffer(name))
	{
		return mDefaultShaderVertexPbrInstance->GetRhiShader()->GetUniformBuffer(name);
	}
	else if (mDefaultShaderFragmentPbrInstance->GetRhiShader()->HasUniformBuffer(name))
	{
		return mDefaultShaderFragmentPbrInstance->GetRhiShader()->GetUniformBuffer(name);
	}
	RHICBufferDesc empty;
	return empty;
}

void RenderContext::FlushStaging()
{
	mFence->Wait(mFenceValue);
	mTransferCmd->GetCmdList()->CloseCommondList();
	mTransferQueue->ExecuteCommandLists(mTransferCmd->GetCmdList());
	mTransferQueue->Signal(mFence, ++mFenceValue);
	mFence->Wait(mFenceValue);
	mBarrierCmd->GetCmdList()->CloseCommondList();
	mGraphicQueue->ExecuteCommandLists(mBarrierCmd->GetCmdList());
	mGraphicQueue->Signal(mFence, ++mFenceValue);
	mFence->Wait(mFenceValue);
	mTransferCmd->Reset();
	mBarrierCmd->Reset();
}

void RenderContext::BeginRenderPass(const RenderPassDesc& desc)
{
	mGraphicCmd->GetCmdList()->BeginRender(desc);
	mCurRenderPass = desc;
}

void RenderContext::EndRenderPass()
{
	mGraphicCmd->GetCmdList()->EndRender();
}

RHIBindingSetPtr RenderContext::CreateBindingset(RHIBindingSetLayoutPtr layout)
{
	RHIBindingSetPtr bindingset = mDevice->CreateBindingSet(mDefaultPool, layout);
	return bindingset;
}

void RenderContext::DrawFullScreen(graphics::MaterialInstanceGraphBase* mat)
{
	DrawMesh(mFullScreenRenderMesh, mat);
}

void RenderContext::DrawMesh(graphics::RenderAssetDataMesh* mesh, graphics::MaterialInstanceGraphBase* mat)
{
	ZoneScoped;
	DrawMeshInstanced(mesh, mat, nullptr, 0, 1);
}

void RenderContext::Dispatch(MaterialInstanceComputeBase* mat, LVector4i dispatchSize)
{
	auto pipeline = mat->GetPipeline();
	mGraphicCmd->GetCmdList()->SetPipelineState(pipeline);
	mat->BindToPipeline(mGraphicCmd->GetCmdList());
	mGraphicCmd->GetCmdList()->Dispatch(dispatchSize.x(), dispatchSize.y(), dispatchSize.z());
}

void RenderContext::DrawMeshBatch(const MeshDrawCommandBatch& meshDrawCommand)
{
	RHIVertexLayout layout = meshDrawCommand.mDrawParameter.mRenderMeshs->GetVertexLayout();
	auto pipeline = meshDrawCommand.mDrawParameter.mMtl->GetPipeline(&layout, mCurRenderPass);
	mGraphicCmd->GetCmdList()->SetPipelineState(pipeline);
	meshDrawCommand.mDrawParameter.mMtl->BindToPipeline(mGraphicCmd->GetCmdList());
	size_t vertexCount = meshDrawCommand.mDrawParameter.mRenderMeshs->GetVertexSize();
	RHIResource* vb = meshDrawCommand.mDrawParameter.mRenderMeshs->GetVertexBuffer();
	size_t indexCount = meshDrawCommand.mDrawParameter.mRenderMeshs->GetIndexSize();
	RHIResource* ib = meshDrawCommand.mDrawParameter.mRenderMeshs->GetIndexBuffer();
	LArray<RHIVertexBufferDesc> descs;
	RHIVertexBufferDesc& vbDesc = descs.emplace_back();
	vbDesc.mOffset = 0;
	vbDesc.mBufferSize = vertexCount * meshDrawCommand.mDrawParameter.mRenderMeshs->GetStridePerVertex();
	vbDesc.mVertexStride = meshDrawCommand.mDrawParameter.mRenderMeshs->GetStridePerVertex();
	vbDesc.mVertexRes = vb;
	mGraphicCmd->GetCmdList()->SetVertexBuffer(descs, 0);
	mGraphicCmd->GetCmdList()->SetIndexBuffer(ib);
	mGraphicCmd->GetCmdList()->SetDrawPrimitiveTopology(RHIPrimitiveTopology::TriangleList);
	mGraphicCmd->GetCmdList()->PushInt32Constant(meshDrawCommand.mRoOffset,0, meshDrawCommand.mDrawParameter.mMtl->GetAsset()->GetBindingSetLayout());
	mGraphicCmd->GetCmdList()->DrawIndexedInstanced((uint32_t)indexCount, meshDrawCommand.mDrawCount, 0, 0, 0);
}

void RenderContext::DrawMeshInstanced(
	RenderAssetDataMesh* mesh,
	MaterialInstanceGraphBase* mat,
	graphics::RHIResource* vertexInputInstanceRes /*= nullptr*/,
	int32_t startInstanceIdx /*= 1*/,
	int32_t instancingSize /*= 1*/
)
{
	ZoneScoped;
	RHIVertexLayout layout = mesh->GetVertexLayout();
	layout.AddVertexElement(VertexElementType::Int, VertexElementUsage::UsageInstanceMessage, 4, 1, VertexElementInstanceType::PerInstance);

	auto pipeline = mat->GetPipeline(&layout, mCurRenderPass);

	mGraphicCmd->GetCmdList()->SetPipelineState(pipeline);
	mat->BindToPipeline(mGraphicCmd->GetCmdList());

	size_t vertexCount = mesh->GetVertexSize();

	RHIResource* vb = mesh->GetVertexBuffer();
	size_t indexCount = mesh->GetIndexSize();
	RHIResource* ib = mesh->GetIndexBuffer();

	LArray<RHIVertexBufferDesc> descs;
	RHIVertexBufferDesc& vbDesc = descs.emplace_back();
	vbDesc.mOffset = 0;
	vbDesc.mBufferSize = vertexCount * mesh->GetStridePerVertex();
	vbDesc.mVertexStride = mesh->GetStridePerVertex();
	vbDesc.mVertexRes = vb;

	if (vertexInputInstanceRes != nullptr)
	{
		RHIVertexBufferDesc& vbInstancingDesc = descs.emplace_back();
		vbInstancingDesc.mOffset = 0;
		vbInstancingDesc.mBufferSize = vertexInputInstanceRes->GetMemoryRequirements().size;
		vbInstancingDesc.mVertexStride = layout.GetSize()[1];
		vbInstancingDesc.mVertexRes = vertexInputInstanceRes;
	}
	else
	{
		RHIVertexBufferDesc& vbInstancingDesc = descs.emplace_back();
		vbInstancingDesc.mOffset = 0;
		vbInstancingDesc.mBufferSize = emptyInstanceBufferSize;
		vbInstancingDesc.mVertexStride = layout.GetSize()[1];
		vbInstancingDesc.mVertexRes = emptyInstanceBuffer.get();
	}

	mGraphicCmd->GetCmdList()->SetVertexBuffer(descs, 0);
	mGraphicCmd->GetCmdList()->SetIndexBuffer(ib);
	switch (((MaterialGraphAsset*)mat->GetAsset())->GetPrimitiveType())
	{
	case RHIPrimitiveTopologyType::Triangle:
		mGraphicCmd->GetCmdList()->SetDrawPrimitiveTopology(RHIPrimitiveTopology::TriangleList);
		break;
	case RHIPrimitiveTopologyType::Line:
		mGraphicCmd->GetCmdList()->SetDrawPrimitiveTopology(RHIPrimitiveTopology::LineList);
		break;
	default:
		break;
	}
	mGraphicCmd->GetCmdList()->DrawIndexedInstanced((uint32_t)indexCount, instancingSize, 0, 0, startInstanceIdx);
}
}



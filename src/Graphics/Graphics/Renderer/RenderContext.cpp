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

RHIShaderBlobPtr ShaderBlobCache::CreateShader(RenderResourceGenerateHelper* mDevice, const RHIShaderDesc& desc)
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
	std::function<RHIShaderBlobPtr(RenderResourceGenerateHelper* device, const RHIShaderDesc &desc)> dataCreateFunc = [&](RenderResourceGenerateHelper* device, const RHIShaderDesc& desc)->RHIShaderBlobPtr
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

void PipelineStateCache::PackShaderToMemory(RenderResourceGenerateHelper* mDevice, luna::LMemoryHash& newHash, const RHIShaderBlob* shaderData)
{
	size_t shader_id = -1;
	if (shaderData != nullptr)
	{
		shader_id = mDevice->GetShaderId(shaderData);
		
	}
	newHash.Combine((uint8_t*)&shader_id, sizeof(size_t));
}

RHIPipelineStatePtr PipelineStateCache::CreatePipelineGraphic(
	RenderResourceGenerateHelper* mDevice,
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
		RenderResourceGenerateHelper* device,
		const RHIPipelineStateGraphDrawDesc& desc,
		const RHIVertexLayout& inputLayout,
		const RenderPassDesc& renderPassDesc)> dataCreateFunc = [&](
			RenderResourceGenerateHelper* device,
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
	RenderResourceGenerateHelper* mDevice,
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
	std::function<RHIPipelineStatePtr(RenderResourceGenerateHelper* device, const RHIPipelineStateComputeDesc& desc)> dataCreateFunc = [&](RenderResourceGenerateHelper* device, const RHIPipelineStateComputeDesc& desc)->RHIPipelineStatePtr
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
	RenderResourceGenerateHelper* mDevice,
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
	std::function<RHICmdSignaturePtr(RenderResourceGenerateHelper* device, RHIPipelineState* pipeline,const LArray<CommandArgDesc>& allCommondDesc)> dataCreateFunc = [&](
		RenderResourceGenerateHelper* device,RHIPipelineState* pipeline,const LArray<CommandArgDesc>& allCommondDesc)->RHICmdSignaturePtr
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

RenderResourceGenerateHelper::RenderResourceGenerateHelper()
{
};

void RenderResourceGenerateHelper::Init()
{
	if (Config_RenderDeviceType.GetValue() == "DirectX12")
		mDeviceType = RenderDeviceType::DirectX12;
	else
		mDeviceType = RenderDeviceType::Vulkan;
	mDevice = GenerateRenderDevice();
	mDevice->InitDeviceData();
	mGraphicQueue = GenerateRenderQueue();
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
		poolDesc.mPoolAllocateSizes[DescriptorHeapType::CBV_SRV_UAV] = 4096*4;
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
}

void RenderResourceGenerateHelper::OnFrameBegin()
{
	ZoneScoped;
	mFGOffset = 0;
	mStagingBufferPool->TickPoolRefresh();
}

void RenderResourceGenerateHelper::OnFrameEnd()
{
	mBindingSetCache.clear();
}

void RenderResourceGenerateHelper::FlushStaging()
{
	if (mAllResGenerateBarrier.size() == 0)
	{
		return;
	}
	mBarrierCmd->Reset();
	mBarrierCmd->GetCmdList()->ResourceBarrierExt(mAllResGenerateBarrier);
	mBarrierCmd->GetCmdList()->CloseCommondList();
	sRenderModule->GetRenderContext()->mGraphicQueue->ExecuteCommandLists(mBarrierCmd->GetCmdList());
}


RHIResourcePtr RenderResourceGenerateHelper::FGCreateTexture(const RHIResDesc& resDesc)
{
	RHIResourcePtr res = _CreateTextureByMemory(resDesc, mFGMemory, mFGOffset);
	const MemoryRequirements& memoryReq = res->GetMemoryRequirements();
	mFGOffset += memoryReq.size;
	return res;
}

RHIResourcePtr RenderResourceGenerateHelper::FGCreateBuffer(const RHIBufferDesc& resDesc)
{
	RHIResourcePtr res = _CreateBufferByMemory(resDesc, mFGMemory, mFGOffset);
	const MemoryRequirements& memoryReq = res->GetMemoryRequirements();
	mFGOffset += memoryReq.size;
	return res;
}

RHIResourcePtr RenderResourceGenerateHelper::_CreateBufferByMemory(const RHIBufferDesc& desc, RHIMemoryPtr targetMemory, size_t& memoryOffset)
{
	assert(desc.mSize != 0);
	RHIBufferDesc resDesc = desc;
	resDesc.mBufferUsage = resDesc.mBufferUsage | RHIBufferUsage::TransferDstBit;
	RHIResourcePtr dstBuffer = mDevice->CreateBufferExt(resDesc);
	dstBuffer->BindMemory(targetMemory, memoryOffset);
	return dstBuffer;
}

RHIResourcePtr RenderResourceGenerateHelper::_CreateTextureByMemory(const RHIResDesc& resDesc, RHIMemoryPtr targetMemory, size_t& memoryOffset)
{
	RHIResDesc newResDesc = resDesc;
	newResDesc.mImageUsage = newResDesc.mImageUsage | RHIImageUsage::TransferDstBit;
	RHIResourcePtr textureRes = mDevice->CreateTextureExt(newResDesc);
	const MemoryRequirements& memoryReq = textureRes->GetMemoryRequirements();
	textureRes->BindMemory(targetMemory, memoryOffset);
	
	return textureRes;
}

RHIResourcePtr RenderResourceGenerateHelper::_CreateBuffer(RHIHeapType memoryType, const RHIBufferDesc& desc, void* initData,size_t initDataSize)
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

RHIResourcePtr RenderResourceGenerateHelper::CreateBuffer(RHIHeapType memoryType, const RHIBufferDesc& resDesc, void* initData, size_t initDataSize)
{
	size_t offset = 0;
	return _CreateBuffer(memoryType,resDesc, initData, initDataSize);
}

RHIResourcePtr RenderResourceGenerateHelper::_CreateTexture(const RHIResDesc& resDesc, void* initData, size_t dataSize, const RHISubResourceCopyDesc& sourceCopyOffset)
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
	mStagingBufferPool->UploadToDstTexture(dataSize, initData, sourceCopyOffset,textureRes, 0);

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
		mAllResGenerateBarrier.push_back(dstBarrier);
	}
	else if (Has(resDesc.mImageUsage, RHIImageUsage::ColorAttachmentBit))
	{
		dstBarrier.mStateBefore = kCommon;
		dstBarrier.mStateAfter = kRenderTarget;
		mAllResGenerateBarrier.push_back(dstBarrier);
		mAllResGenerateBarrier.push_back(dstBarrier);
	}
	return textureRes;
}

RHIResourcePtr RenderResourceGenerateHelper::CreateTexture(const RHIResDesc& resDesc, void* initData, size_t dataSize, const RHISubResourceCopyDesc sourceCopyOffset)
{
	size_t offset = 0;
	return _CreateTexture(resDesc, initData, dataSize, sourceCopyOffset);
}

RHIShaderBlobPtr RenderResourceGenerateHelper::CreateShader(const RHIShaderDesc& desc)
{
	return mShaderBlobCache.CreateShader(this,desc);
}

size_t RenderResourceGenerateHelper::GetShaderId(const RHIShaderBlob* shader)
{
	return mShaderBlobCache.GetDataGlobelId(shader);
}

RHIPipelineStatePtr RenderResourceGenerateHelper::CreatePipelineGraphic(
	const RHIPipelineStateGraphDrawDesc& desc,
	const RHIVertexLayout& inputLayout,
	const RenderPassDesc& renderPassDesc
)
{
	return mPipelineCache.CreatePipelineGraphic(this, desc, inputLayout, renderPassDesc);
}

RHIPipelineStatePtr RenderResourceGenerateHelper::CreatePipelineCompute(const RHIPipelineStateComputeDesc& desc)
{
	return mPipelineCache.CreatePipelineCompute(this, desc);
}

size_t RenderResourceGenerateHelper::GetPipelineId(const RHIPipelineState* pipeline)
{
	return mPipelineCache.GetDataGlobelId(pipeline);
}

RHICmdSignaturePtr RenderResourceGenerateHelper::CreateCmdSignature(RHIPipelineState* pipeline, const LArray<CommandArgDesc>& allCommondDesc)
{
	return mCmdSignatureCache.CreateCmdSignature(this, pipeline, allCommondDesc);
}

size_t RenderResourceGenerateHelper::GetCmdSignatureId(const RHICmdSignature* pipeline)
{
	return mCmdSignatureCache.GetDataGlobelId(pipeline);
}

RHICBufferDesc RenderResourceGenerateHelper::GetDefaultShaderConstantBufferDesc(ShaderParamID name)
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

RHIBindingSetPtr RenderResourceGenerateHelper::CreateBindingset(RHIBindingSetLayoutPtr layout)
{
	RHIBindingSetPtr bindingset = mDevice->CreateBindingSet(mDefaultPool, layout);
	return bindingset;
}


void RenderCommandGenerateHelper::Init(RHIDevice* curDevice)
{
	
}

void RenderCommandGenerateHelper::BindDrawCommandPassDesc(RHICmdList* cmdList, const RenderPassDesc& desc)
{
	cmdList->BindDrawCommandPassDesc(desc);
}

void RenderCommandGenerateHelper::EndRenderPass(RHICmdList* cmdList)
{
	cmdList->EndRender();
}



void RenderCommandGenerateHelper::DrawFullScreen(RHICmdList* cmdList, graphics::MaterialInstanceGraphBase* mat)
{
	DrawMesh(cmdList,mFullScreenRenderMesh, mat);
}

void RenderCommandGenerateHelper::DrawMesh(RHICmdList* cmdList, graphics::RenderAssetDataMesh* mesh, graphics::MaterialInstanceGraphBase* mat)
{
	ZoneScoped;
	DrawMeshInstanced(cmdList,mesh, mat, 1);
}

void RenderCommandGenerateHelper::Dispatch(RHICmdList* cmdList, MaterialInstanceComputeBase* mat, LVector4i dispatchSize)
{
	auto pipeline = mat->GetPipeline();
	cmdList->SetPipelineState(pipeline);
	mat->BindToPipeline(cmdList);
	cmdList->Dispatch(dispatchSize.x(), dispatchSize.y(), dispatchSize.z());
}

void RenderCommandGenerateHelper::DrawMeshBatch(RHICmdList* cmdList, const MeshDrawCommandBatch& meshDrawCommand)
{
	RHIVertexLayout layout = meshDrawCommand.mDrawParameter.mRenderMeshs->GetVertexLayout();
	auto pipeline = meshDrawCommand.mDrawParameter.mMtl->GetPipeline(&layout, cmdList->GetCurPassDesc());
	cmdList->SetPipelineState(pipeline);
	meshDrawCommand.mDrawParameter.mMtl->BindToPipeline(cmdList);
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
	cmdList->SetVertexBuffer(descs, 0);
	cmdList->SetIndexBuffer(ib);
	cmdList->SetDrawPrimitiveTopology(RHIPrimitiveTopology::TriangleList);
	LVector4i data;
	data.x() = meshDrawCommand.mRoOffset;
	cmdList->PushInt32Constant(0,&data,sizeof(data), meshDrawCommand.mDrawParameter.mMtl->GetAsset()->GetBindingSetLayout(), RHICmdListType::Graphic3D);
	cmdList->DrawIndexedInstanced((uint32_t)indexCount, meshDrawCommand.mDrawCount, 0, 0, 0);
}

void RenderCommandGenerateHelper::DrawMeshInstanced(
	RHICmdList* cmdList,
	RenderAssetDataMesh* mesh,
	MaterialInstanceGraphBase* mat,
	int32_t instancingSize /*= 1*/
)
{
	ZoneScoped;
	RHIVertexLayout layout = mesh->GetVertexLayout();

	auto pipeline = mat->GetPipeline(&layout, cmdList->GetCurPassDesc());

	cmdList->SetPipelineState(pipeline);
	mat->BindToPipeline(cmdList);

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

	cmdList->SetVertexBuffer(descs, 0);
	cmdList->SetIndexBuffer(ib);
	switch (((MaterialGraphAsset*)mat->GetAsset())->GetPrimitiveType())
	{
	case RHIPrimitiveTopologyType::Triangle:
		cmdList->SetDrawPrimitiveTopology(RHIPrimitiveTopology::TriangleList);
		break;
	case RHIPrimitiveTopologyType::Line:
		cmdList->SetDrawPrimitiveTopology(RHIPrimitiveTopology::LineList);
		break;
	default:
		break;
	}
	cmdList->DrawIndexedInstanced((uint32_t)indexCount, instancingSize, 0, 0, 0);
}
}



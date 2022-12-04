#pragma once

#include "render/pch.h"
#include "render/rhi/rhi_types.h"
#include "render/rhi/rhi_device.h"


namespace luna::render
{

struct RenderObject;
class SubMesh;
class ShaderAsset;

RENDER_API CONFIG_DECLARE(LString, Render, RenderDeviceType, 2);

using PipelinePair = std::pair< RHIPipelineStatePtr, RHIBindingSetPtr>;

enum class RenderDeviceType : uint8_t
{
	Unkown = 0,
	DirectX12 = 1,
	Vulkan = 2,
};

size_t GetOffset(size_t offset, uint16_t aliment);

#define PARAM_ID(name) static luna::render::ShaderParamID ParamID_##name = luna::LString(#name).Hash();

struct PackedParams
{
	void Clear()
	{
		mParams.clear();
	}

	void PushShaderParam(ShaderParamID id, RHIView* view)
	{
		auto& it = mParams.emplace_back();
		it.first = id;
		it.second = view;
	}

	std::vector<std::pair<ShaderParamID, RHIViewPtr>> mParams;	
};

class RENDER_API RenderDevice 
{
public:
	RenderDevice() {};
	virtual ~RenderDevice() {};

	void Init();

	RHIRenderQueuePtr mGraphicQueue;
	RHIRenderQueuePtr mTransferQueue;
	RHIDevice* mDevice;
	RenderDeviceType    mDeviceType = RenderDeviceType::DirectX12;
	RHIGraphicCmdListPtr mGraphicCmd;
	RHIGraphicCmdListPtr mTransferCmd;
public:
	//Frame Graph API
public:

	PipelinePair CreatePipelineState(ShaderAsset* shaderPass);
	PipelinePair CreatePipelineState(const RHIPipelineStateDesc& desc);
	RHIResourcePtr CreateFGTexture(const RHITextureDesc& textureDesc, const RHIResDesc& resDesc, void* initData = nullptr, size_t dataSize = 0);
	RHIResourcePtr CreateFGBuffer(const RHIBufferDesc& resDesc, void* initData);

	RHIResourcePtr CreateBuffer(const RHIBufferDesc& resDesc, void* initData = nullptr);
	RHIResourcePtr CreateTexture(const RHITextureDesc& textureDesc, const RHIResDesc& resDesc, void* initData = nullptr, size_t dataSize = 0);

	RHIViewPtr CreateView(const ViewDesc& view);

	void UpdateConstantBuffer(RHIResourcePtr target, void* data, size_t dataSize);
	void FlushStaging();

	void BeginRendering(const RenderPassDesc&);
	void EndRenderPass();

	void DrawRenderOBject(render::RenderObject* mesh, render::ShaderAsset* shader, PackedParams* params);

	void OnFrameBegin();
	void OnFrameEnd();
private:
	RHIMemoryPtr				mFGMemory;
	size_t						mFGOffset = 0;

private:
	//‘⁄Staging Buffer…œ∑÷≈‰
	RHIResourcePtr _AllocateStagingBuffer(void* initData, size_t dataSize, RHIResourcePtr resMemoryLayout);

	RHIResourcePtr _CreateBuffer(const RHIBufferDesc& resDesc, 
		void* initData, RHIMemoryPtr targetMemory, size_t& memoryOffset);
	RHIResourcePtr _CreateTexture(const RHITextureDesc& textureDesc, const RHIResDesc& resDesc, 
		void* initData , size_t dataSize, RHIMemoryPtr targetMemory, size_t& memoryOffset);

public:
	using PipelineCacheKey = std::pair<ShaderAsset*, size_t>;
	RenderPassDesc mCurRenderPass;
	std::map<PipelineCacheKey, PipelinePair> mPipelineCache;

	RHIPipelineStatePtr mLastPipline;

	RHIMemoryPtr mStagingMemory;
	std::vector<RHIResourcePtr> historyStagingBuffer;
	size_t mStagingOffset = 0;

	std::map<RHIBindingSetLayout*, RHIBindingSetPtr> mDescriptorsCache;	
		

	RHIDescriptorPoolPtr        mDefaultPool;

	std::vector<RHIResourcePtr> mStagingHolds;
	uint64_t                    mFenceValue = 0;
	RHIFencePtr                 mFence;
	RHIResourcePtr              mClampSampler;
	RHIViewPtr                  mClampSamplerView;
	//std::vector<TRHIPtr<DX12Descriptor>> mTempHolds;
private:


};

}
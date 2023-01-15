#pragma once

#include "core/core_min.h"

#include "render/render_config.h"
#include "render/forward_types.h"
#include "render/rhi/rhi_types.h"
#include "render/rhi/rhi_device.h"
#include "render/rhi/rhi_render_pass.h"


namespace luna::render
{

RENDER_API CONFIG_DECLARE(LString, Render, RenderDeviceType, 2);

using PipelinePair = std::pair< RHIPipelineStatePtr, RHIBindingSetPtr>;
class MaterialInstance;

enum class RenderDeviceType : uint8_t
{
	Unkown = 0,
	DirectX12 = 1,
	Vulkan = 2,
};

size_t GetOffset(size_t offset, uint16_t aliment);

#define PARAM_ID(name) static luna::render::ShaderParamID ParamID_##name = luna::LString(#name).Hash();

class DynamicMemoryBuffer
{
	RHIDevice*                  mDevice;
	RHIBufferUsage              mBufferUsage;
	RHIMemoryPtr                mFullMemory;
	std::vector<RHIResourcePtr> mhistoryBuffer;
	size_t                      mBufferOffset = 0;
	size_t                      mMaxSize;
public:
	DynamicMemoryBuffer(size_t maxSize, RHIBufferUsage bufferUsage) :mMaxSize(maxSize), mBufferUsage(bufferUsage) {};
	void Init(RHIDevice* device,const RHIHeapType memoryHeapType, const int32_t memoryType);
	RHIResource* AllocateNewBuffer(void* initData, size_t dataSize, size_t bufferResSize);
	void Reset();
};



class RENDER_API RenderDevice 
{
public:
	RenderDevice();
	virtual ~RenderDevice() {};

	void Init();

	RHIRenderQueuePtr    mGraphicQueue;
	RHIRenderQueuePtr    mTransferQueue;
	RHIDevice*           mDevice;
	RenderDeviceType     mDeviceType = RenderDeviceType::DirectX12;
	RHIGraphicCmdListPtr mGraphicCmd;
	RHIGraphicCmdListPtr mTransferCmd;
	RHIGraphicCmdListPtr mBarrierCmd;
public:
	//Frame Graph API
public:

	PipelinePair CreatePipelineState(MaterialInstance* mat, const RenderPassDesc& desc, RHIVertexLayout* layout);
	PipelinePair CreatePipelineState(const RHIPipelineStateDesc& desc);
	RHIResourcePtr CreateFGTexture(const RHITextureDesc& textureDesc, const RHIResDesc& resDesc, void* initData = nullptr, size_t dataSize = 0);
	RHIResourcePtr CreateFGBuffer(const RHIBufferDesc& resDesc, void* initData);

	RHIResourcePtr CreateBuffer(const RHIBufferDesc& resDesc, void* initData = nullptr);
	RHIResourcePtr CreateTexture(const RHITextureDesc& textureDesc, const RHIResDesc& resDesc, void* initData = nullptr, size_t dataSize = 0);
	RHIResource* CreateInstancingBufferByRenderObjects(LArray<RenderObject*> RenderObjects);

	RHIViewPtr CreateView(const ViewDesc& view);

	void UpdateConstantBuffer(RHIResourcePtr target, void* data, size_t dataSize);
	void FlushStaging();
	void FlushFrameInstancingBuffer() { mInstancingIdMemory.Reset(); };

	void BeginRenderPass(const RenderPassDesc&);
	void EndRenderPass();

	void DrawRenderOBject(render::RenderObject* mesh, render::MaterialInstance* mat, PackedParams* params, render::RHIResource* instanceMessage = nullptr,int32_t instancingSize = 1);
	void DrawMesh(render::SubMesh*, render::MaterialInstance* mat, PackedParams* params, render::RHIResource* instanceMessage = nullptr, int32_t instancingSize = 1);

	void OnFrameBegin();
	void OnFrameEnd();
private:
	using PipelineCacheKey = std::pair<MaterialInstance*, size_t>;

	PipelineCacheKey PreparePipeline(MaterialInstance* mat, render::SubMesh* mesh, PackedParams* params);

	RHIMemoryPtr				mFGMemory;
	size_t						mFGOffset = 0;

private:
	RHIResourcePtr _CreateBuffer(const RHIBufferDesc& resDesc, 
		void* initData, RHIMemoryPtr targetMemory, size_t& memoryOffset);
	RHIResourcePtr _CreateTexture(const RHITextureDesc& textureDesc, const RHIResDesc& resDesc, 
		void* initData , size_t dataSize, RHIMemoryPtr targetMemory, size_t& memoryOffset);

public:
	RenderPassDesc mCurRenderPass;
	std::map<PipelineCacheKey, PipelinePair> mPipelineCache;

	RHIPipelineStatePtr mLastPipline;

	DynamicMemoryBuffer mStagingMemory;
	DynamicMemoryBuffer mInstancingIdMemory;

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
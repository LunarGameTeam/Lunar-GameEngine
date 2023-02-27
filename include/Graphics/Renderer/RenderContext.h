#pragma once

#include "Core/CoreMin.h"

#include "Graphics/RenderConfig.h"
#include "Graphics/RenderTypes.h"

#include "Graphics/RHI/RHITypes.h"
#include "Graphics/RHI/RHIDevice.h"
#include "Graphics/RHI/RHIRenderPass.h"

#include "Graphics/Asset/ShaderAsset.h"


namespace luna::render
{

RENDER_API CONFIG_DECLARE(LString, Render, RenderDeviceType, 2);

enum class RenderDeviceType : uint8_t
{
	Unkown = 0,
	DirectX12 = 1,
	Vulkan = 2,
};

size_t GetOffset(size_t offset, uint16_t aliment);

class RENDER_API DynamicMemoryBuffer
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



class RENDER_API RenderContext final : NoCopy
{
public:
	RenderContext();
	~RenderContext() = default;
	void Init();

	RHIGraphicCmdListPtr mGraphicCmd;
	RHIRenderQueuePtr    mGraphicQueue;
	RHIGraphicCmdListPtr mTransferCmd;
	RHIRenderQueuePtr    mTransferQueue;
	RHIGraphicCmdListPtr mBarrierCmd;

	RHIDevice*           mDevice;
	RenderDeviceType     mDeviceType = RenderDeviceType::DirectX12;

	uint64_t             mFenceValue = 0;
	RHIFencePtr          mFence;

	//----Frame Graph API Begin----
public:	
	RHIResourcePtr FGCreateTexture(const RHITextureDesc& textureDesc, const RHIResDesc& resDesc, void* initData = nullptr, size_t dataSize = 0);
	RHIResourcePtr FGCreateBuffer(const RHIBufferDesc& resDesc, void* initData);
private:
	RHIMemoryPtr				mFGMemory;
	size_t						mFGOffset = 0;
	//----Frame Graph API End----

public:
	//----Resource Graph API Begin----
	RHIResourcePtr CreateBuffer(const RHIBufferDesc& resDesc, void* initData = nullptr);
	RHIResourcePtr CreateTexture2D(uint32_t width, uint32_t height, RHITextureFormat format = RHITextureFormat::R8G8B8A8_UNORM,void* initData = nullptr, size_t dataSize = 0);
	RHIResourcePtr CreateTexture(const RHITextureDesc& textureDesc, const RHIResDesc& resDesc, void* initData = nullptr, size_t dataSize = 0);
	RHIResource* CreateInstancingBufferByRenderObjects(const LArray<RenderObject*>& RenderObjects);
	void UpdateConstantBuffer(RHIResourcePtr target, void* data, size_t dataSize);
	//----Resource Graph API End----

	//----Draw Graph API Begin----
public:
	void BeginRenderPass(const RenderPassDesc&);
	void EndRenderPass();

	void DrawRenderOBject(render::RenderObject* mesh, render::MaterialInstance* mat, PackedParams* params);
	void DrawMesh(render::SubMesh*, render::MaterialInstance* mat, PackedParams* params);
	void DrawMeshInstanced(render::SubMesh*, render::MaterialInstance* mat, PackedParams* params, render::RHIResource* instanceMessage = nullptr, int32_t startInstanceIdx = 1, int32_t instancingSize = 1);
private:
	using PipelineCacheKey = std::pair < MaterialInstance*, size_t>;
	RenderPassDesc                              mCurRenderPass;
	LMap<size_t, RHIBindingSetPtr>              mBindingSetCache;
	LMap<PipelineCacheKey, RHIPipelineStatePtr> mPipelineCache;
	RHIPipelineStatePtr                         mLastPipline;

	//----Draw Graph API End----

public:
	void OnFrameBegin();
	void OnFrameEnd();
	void FlushStaging();

	RHIDescriptorPoolPtr GetDefaultDescriptorPool() { return mDefaultPool; }

	RHIBindingSetLayoutPtr mViewBindingSet;
	SharedPtr<ShaderAsset>        mDefaultShader;
private:
	RHIPipelineStatePtr CreatePipelineState(MaterialInstance* mat, const RenderPassDesc& desc, RHIVertexLayout* layout);

	using PipelineCacheKey = std::pair<MaterialInstance*, size_t>;

	RHIPipelineStatePtr GetPipeline(MaterialInstance* mat, RHIVertexLayout* layout);
	RHIBindingSetPtr GetBindingSet(RHIPipelineState* pipeline, PackedParams* packparams);


private:
	
	void FlushFrameInstancingBuffer() { mInstancingIdMemory.Reset(); };

	RHIResourcePtr _CreateBuffer(const RHIBufferDesc& resDesc, 
		void* initData, RHIMemoryPtr targetMemory, size_t& memoryOffset);
	RHIResourcePtr _CreateTexture(const RHITextureDesc& textureDesc, const RHIResDesc& resDesc, 
		void* initData , size_t dataSize, RHIMemoryPtr targetMemory, size_t& memoryOffset);

private:

	DynamicMemoryBuffer                         mStagingMemory;
	DynamicMemoryBuffer                         mInstancingIdMemory;

	RHIDescriptorPoolPtr                        mDefaultPool;

public:
	//Samplers
	RHIResourcePtr                              mClampSampler;
	RHIViewPtr                                  mClampSamplerView;
	RHIResourcePtr                              mRepeatSampler;
	RHIViewPtr                                  mRepeatSamplerView;
private:

};

}
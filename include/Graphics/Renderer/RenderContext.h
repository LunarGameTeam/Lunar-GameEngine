﻿#pragma once

#include "Core/CoreMin.h"
#include "Core/Foundation/MemoryHash.h"
#include "Graphics/RenderConfig.h"
#include "Graphics/RenderTypes.h"

#include "Graphics/RHI/RHITypes.h"
#include "Graphics/RHI/RHIDevice.h"
#include "Graphics/RHI/RHIRenderPass.h"
#include "Graphics/RHI/RHICmdList.h"

#include "Graphics/Asset/ShaderAsset.h"
#include "Graphics/Renderer/RenderMesh.h"

namespace luna::graphics
{

RENDER_API CONFIG_DECLARE(LString, Render, RenderDeviceType, 2);
class RenderContext;
class RhiObjectCache
{
	LQueue<size_t> emptyID;

	LUnorderedMap<LMemoryHash, TRHIPtr<RHIObject>> mData;

	LUnorderedMap<const RHIObject*, size_t> mDataGlobelId;
public:
	size_t GetDataGlobelId(const RHIObject* customData);

	void ReleaseData(LMemoryHash& newHash);
protected:

	template <typename T, typename... Args>
	TRHIPtr<T> CreateRHIObject(
		RenderContext* device,
		Args... inputDesc,
		std::function<void(LMemoryHash&,Args...)> &dataHashFunc,
		std::function<TRHIPtr<T>(RenderContext* device, Args...)> dataCreateFunc
	)
	{
		LMemoryHash newHash;
		dataHashFunc(newHash, inputDesc...);
		auto dataExist = mData.find(newHash);
		if (dataExist != mData.end())
		{
			return dataExist->second;
		}
		TRHIPtr<T> newData = dataCreateFunc(device,inputDesc...);
		mData.insert({ newHash ,newData });
		if (emptyID.empty())
		{
			size_t newID = mDataGlobelId.size();
			mDataGlobelId.insert({ newData ,newID });
		}
		else
		{
			size_t newID = emptyID.front();
			emptyID.pop();
			mDataGlobelId.insert({ newData ,newID });
		}
		return newData;
	}
};

class ShaderBlobCache:public RhiObjectCache
{
public:
	RHIShaderBlobPtr CreateShader(RenderContext* mDevice, const RHIShaderDesc& desc);
};

class PipelineStateCache :public RhiObjectCache
{
public:
	RHIPipelineStatePtr CreatePipelineGraphic(
		RenderContext* mDevice,
		const RHIPipelineStateGraphDrawDesc& desc,
		const RHIVertexLayout& inputLayout,
		const RenderPassDesc& renderPassDesc
	);

	RHIPipelineStatePtr CreatePipelineCompute(
		RenderContext* mDevice,
		const RHIPipelineStateComputeDesc& desc
	);
private:
	void PackShaderToMemory(RenderContext* mDevice, luna::LMemoryHash& newHash, const RHIShaderBlob* shaderData);
};

class CmdSignatureCache :public RhiObjectCache
{
public:
	RHICmdSignaturePtr CreateCmdSignature(
		RenderContext* mDevice,
		RHIPipelineState* pipeline,
		const LArray<CommandArgDesc>& allCommondDesc
	);
};

enum class RenderDeviceType : uint8_t
{
	Unkown = 0,
	DirectX12 = 1,
	Vulkan = 2,
};

size_t GetOffset(size_t offset, uint16_t aliment);

//class RENDER_API RHIDynamicMemory
//{
//	RHIDevice*             mDevice;
//	RHIBufferUsage         mBufferUsage;
//	RHIMemoryPtr           mFullMemory;
//	LArray<RHIResourcePtr> mhistoryBuffer;
//	size_t                 mBufferOffset = 0;
//	size_t                 mMaxSize      = 0;
// public:
//	RHIDynamicMemory(size_t maxSize, RHIBufferUsage bufferUsage) : mMaxSize(maxSize), mBufferUsage(bufferUsage) { };
//
//	void         Init(RHIDevice* device, const RHIHeapType memoryHeapType, const int32_t memoryType);
//	RHIResource* AllocateNewBuffer(void* initData, size_t dataSize, size_t bufferResSize);
//	void         Reset();
//};

struct StaticSampler
{
	void Init(SamplerDesc& desc, ViewDesc& view);

	RHIResourcePtr mSampler;
	RHIViewPtr     mView;	
};
class RENDER_API CommandArgBufferPool
{
	LMemoryHash mCommandSignatureHash;
	RHICmdSignaturePtr mCommandSignature;
	LArray<RHICmdArgBuffer> mArgBufferPool;
	LQueue<size_t> mEmptyBuffer;
public:
	CommandArgBufferPool(
		RHIPipelineState* pipeline,
		const LArray<CommandArgDesc>& allCommondDesc
	);

};


class RENDER_API RenderContext final : NoCopy
{
public:
	RenderContext();
	~RenderContext() = default;
	void Init();
	
	RHISinglePoolSingleCmdListPtr mGraphicCmd;
	RHIRenderQueuePtr    mGraphicQueue;
	RHISinglePoolSingleCmdListPtr mTransferCmd;
	RHIRenderQueuePtr    mTransferQueue;
	RHISinglePoolSingleCmdListPtr mBarrierCmd;
	RHIDevicePtr          mDevice;
	RenderDeviceType     mDeviceType = RenderDeviceType::DirectX12;
	uint64_t             mFenceValue = 0;
	RHIFencePtr          mFence;

	//----Frame Graph API Begin----
public:	
	RHIResourcePtr FGCreateTexture(const RHITextureDesc& textureDesc, const RHIResDesc& resDesc);
	RHIResourcePtr FGCreateBuffer(const RHIBufferDesc& resDesc);
private:
	RHIMemoryPtr				mFGMemory;
	size_t						mFGOffset = 0;
	//----Frame Graph API End----

public:
	//----Resource Graph API Begin----
	RHIResourcePtr CreateBuffer(RHIHeapType memoryType, const RHIBufferDesc& resDesc, void* initData = nullptr, size_t initDataSize = 0);
	RHIResourcePtr CreateTexture2D(uint32_t width, uint32_t height, RHITextureFormat format = RHITextureFormat::R8G8B8A8_UNORM,void* initData = nullptr, size_t dataSize = 0);
	RHIResourcePtr CreateTexture(const RHITextureDesc& textureDesc, const RHIResDesc& resDesc, void* initData = nullptr, size_t dataSize = 0);
	RHIResource* CreateInstancingBufferByRenderObjects(const LArray<RenderObject*>& RenderObjects);
	void UpdateConstantBuffer(RHIResourcePtr target, void* data, size_t dataSize);
	
	RHIShaderBlobPtr CreateShader(const RHIShaderDesc& desc);
	size_t GetShaderId(const RHIShaderBlob* shader);

	RHIPipelineStatePtr CreatePipelineGraphic(
		const RHIPipelineStateGraphDrawDesc& desc,
		const RHIVertexLayout& inputLayout,
		const RenderPassDesc& renderPassDesc
	);

	RHIPipelineStatePtr CreatePipelineCompute(const RHIPipelineStateComputeDesc& desc);

	size_t GetPipelineId(const RHIPipelineState* pipeline);
	RHICmdSignaturePtr CreateCmdSignature(RHIPipelineState* pipeline, const LArray<CommandArgDesc>& allCommondDesc);
	size_t GetCmdSignatureId(const RHICmdSignature* pipeline);
	//----Resource Graph API End----

	RHIBindingSetPtr CreateBindingset(RHIBindingSetLayoutPtr layout);

	//----Draw Graph API Begin----
public:
	void BeginRenderPass(const RenderPassDesc&);
	void EndRenderPass();

	void DrawMesh(graphics::RenderMeshBase*, graphics::MaterialInstance* mat);

	void DrawMeshInstanced(
		RenderMeshBase*, 
		MaterialInstance* mat,
		RHIResource* vertexInputInstanceRes = nullptr, 
		int32_t startInstanceIdx = 1,
		int32_t instancingSize = 1);

	void Dispatch(MaterialInstance* mat,LVector4i dispatchSize);

private:
	using PipelineCacheKey = std::pair < MaterialInstance*, size_t>;
	RenderPassDesc                              mCurRenderPass;
	LMap<size_t, RHIBindingSetPtr>              mBindingSetCache;
	LMap<size_t, RHICmdSignaturePtr>            mCommandSignatureCache;
	ShaderBlobCache mShaderBlobCache;
	PipelineStateCache mPipelineCache;
	CmdSignatureCache mCmdSignatureCache;
	//----Draw Graph API End----

public:
	void OnFrameBegin();
	void OnFrameEnd();
	void FlushStaging();

	RHIDescriptorPoolPtr GetDefaultDescriptorPool() { return mDefaultPool; }

	RHICBufferDesc GetDefaultShaderConstantBufferDesc(ShaderParamID name);

	RHIBindingSetLayoutPtr mViewBindingSet;
	SharedPtr<ShaderAsset> mDefaultShader;

	SharedPtr<LShaderInstance> mDefaultShaderVertexInstance;
	SharedPtr<LShaderInstance> mDefaultShaderFragmentInstance;

	SharedPtr<ShaderAsset> mDefaultShaderPbr;
	SharedPtr<LShaderInstance> mDefaultShaderVertexPbrInstance;
	SharedPtr<LShaderInstance> mDefaultShaderFragmentPbrInstance;

private:

	RHIResourcePtr _CreateBuffer(RHIHeapType memoryType, const RHIBufferDesc& resDesc, void* initData, size_t initDataSize);
	RHIResourcePtr _CreateTexture(const RHITextureDesc& textureDesc, const RHIResDesc& resDesc, void* initData , size_t dataSize);

	RHIResourcePtr _CreateBufferByMemory(const RHIBufferDesc& desc, RHIMemoryPtr targetMemory, size_t& memoryOffset);
	RHIResourcePtr _CreateTextureByMemory(const RHITextureDesc& textureDesc, const RHIResDesc& resDesc, RHIMemoryPtr targetMemory, size_t& memoryOffset);

private:
	std::shared_ptr<RHIStagingBufferPool> mStagingBufferPool;

	RHIDescriptorPoolPtr                        mDefaultPool;

	size_t                                   emptyInstanceBufferSize;
	RHIResourcePtr                           emptyInstanceBuffer;
	RHIResourcePtr                           renderObjectInstancingBuffer;
public:
	//Samplers
	StaticSampler mClamp;
	StaticSampler mRepeat;
	
};

}
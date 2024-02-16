#pragma once

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
#include "Graphics/RenderAssetManager/RenderAssetManager.h"
#include "Graphics/Renderer/MaterialInstance.h"

namespace luna::graphics
{

RENDER_API CONFIG_DECLARE(LString, Render, RenderDeviceType, 2);
class RenderResourceGenerateHelper;
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
		RenderResourceGenerateHelper* device,
		Args... inputDesc,
		std::function<void(LMemoryHash&,Args...)> &dataHashFunc,
		std::function<TRHIPtr<T>(RenderResourceGenerateHelper* device, Args...)> dataCreateFunc
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
	RHIShaderBlobPtr CreateShader(RenderResourceGenerateHelper* mDevice, const RHIShaderDesc& desc);
};

class PipelineStateCache :public RhiObjectCache
{
public:
	RHIPipelineStatePtr CreatePipelineGraphic(
		RenderResourceGenerateHelper* mDevice,
		const RHIPipelineStateGraphDrawDesc& desc,
		const RHIVertexLayout& inputLayout,
		const RenderPassDesc& renderPassDesc
	);

	RHIPipelineStatePtr CreatePipelineCompute(
		RenderResourceGenerateHelper* mDevice,
		const RHIPipelineStateComputeDesc& desc
	);
private:
	void PackShaderToMemory(RenderResourceGenerateHelper* mDevice, luna::LMemoryHash& newHash, const RHIShaderBlob* shaderData);
};

class CmdSignatureCache :public RhiObjectCache
{
public:
	RHICmdSignaturePtr CreateCmdSignature(
		RenderResourceGenerateHelper* mDevice,
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

struct MeshDrawCommandHashKey
{
	RenderAssetDataMesh* mRenderMeshs = nullptr;
	MaterialInstanceGraphBase* mMtl = nullptr;
	bool operator==(const MeshDrawCommandHashKey& key2) const{
		if (mRenderMeshs->mID != key2.mRenderMeshs->mID)
		{
			return false;
		}
		if (mMtl != key2.mMtl)
		{
			return false;
		}
		return true;
	}
};

struct MeshDrawCommandBatch
{
	MeshDrawCommandHashKey mDrawParameter;
	size_t mRoOffset = 0;
	size_t mDrawCount = 0;
};

class RENDER_API RenderResourceGenerateHelper final : NoCopy
{
	LArray<ResourceBarrierDesc> mAllResGenerateBarrier;
	RHISinglePoolSingleCmdListPtr mBarrierCmd;
public:
	RenderResourceGenerateHelper();
	virtual ~RenderResourceGenerateHelper() = default;
	void Init();

	RHIDevicePtr          mDevice;
	RHIRenderQueuePtr    mGraphicQueue;
	RenderDeviceType     mDeviceType = RenderDeviceType::DirectX12;

	//----Frame Graph API Begin----
public:	
	RHIResourcePtr FGCreateTexture(const RHIResDesc& resDesc);
	RHIResourcePtr FGCreateBuffer(const RHIBufferDesc& resDesc);
private:
	RHIMemoryPtr				mFGMemory;
	size_t						mFGOffset = 0;
	//----Frame Graph API End----

public:
	//----Resource Graph API Begin----
	RHIResourcePtr CreateBuffer(RHIHeapType memoryType, const RHIBufferDesc& resDesc, void* initData = nullptr, size_t initDataSize = 0);
	RHIResourcePtr CreateTexture(const RHIResDesc& resDesc, void* initData = nullptr, size_t dataSize = 0, const RHISubResourceCopyDesc sourceCopyOffset = {});
	
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
	

private:
	using PipelineCacheKey = std::pair < MaterialInstance*, size_t>;
	RenderPassDesc                              mCurRenderPass;
	LMap<size_t, RHIBindingSetPtr>              mBindingSetCache;
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

	//RHIBindingSetLayoutPtr mViewBindingSet;
	SharedPtr<ShaderAsset> mDefaultShader;

	SharedPtr<LShaderInstance> mDefaultShaderVertexInstance;
	SharedPtr<LShaderInstance> mDefaultShaderFragmentInstance;

	SharedPtr<ShaderAsset> mDefaultShaderPbr;
	SharedPtr<LShaderInstance> mDefaultShaderVertexPbrInstance;
	SharedPtr<LShaderInstance> mDefaultShaderFragmentPbrInstance;

private:

	RHIResourcePtr _CreateBuffer(RHIHeapType memoryType, const RHIBufferDesc& resDesc, void* initData, size_t initDataSize);
	RHIResourcePtr _CreateTexture(const RHIResDesc& resDesc, void* initData , size_t dataSize, const RHISubResourceCopyDesc& sourceCopyOffset);

	RHIResourcePtr _CreateBufferByMemory(const RHIBufferDesc& desc, RHIMemoryPtr targetMemory, size_t& memoryOffset);
	RHIResourcePtr _CreateTextureByMemory(const RHIResDesc& resDesc, RHIMemoryPtr targetMemory, size_t& memoryOffset);

private:
	std::shared_ptr<RHIStagingBufferPool> mStagingBufferPool;

	RHIDescriptorPoolPtr                        mDefaultPool;
public:
	//Samplers
	StaticSampler mClamp;
	StaticSampler mRepeat;
	
};

class RENDER_API RenderCommandGenerateHelper
{
	RenderAssetDataMesh* mFullScreenRenderMesh = nullptr;
public:
	void Init(RHIDevice* curDevice);

	void BindDrawCommandPassDesc(RHICmdList* cmdList,const RenderPassDesc&);

	void EndRenderPass(RHICmdList* cmdList);

	void DrawMesh(RHICmdList* cmdList, graphics::RenderAssetDataMesh* mesh, graphics::MaterialInstanceGraphBase* mat);

	void DrawFullScreen(RHICmdList* cmdList, graphics::MaterialInstanceGraphBase* mat);

	void DrawMeshInstanced(
		RHICmdList* cmdList,
		RenderAssetDataMesh* mesh,
		MaterialInstanceGraphBase* mat,
		int32_t instancingSize = 1);

	void DrawMeshBatch(RHICmdList* cmdList, const MeshDrawCommandBatch& meshDrawCommand);

	void Dispatch(RHICmdList* cmdList, MaterialInstanceComputeBase* mat, LVector4i dispatchSize);
};


}
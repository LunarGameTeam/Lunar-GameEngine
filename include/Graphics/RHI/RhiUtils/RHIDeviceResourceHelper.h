#pragma once

#include "Graphics/RHI/RHIPch.h"
#include "Graphics/RHI/RHIResource.h"
#include "Graphics/RHI/RhiUtils/RHIStageBufferPool.h"

namespace luna::graphics
{
	class RENDER_API DeviceResourceGenerateHelper final : NoCopy
	{
		LArray<ResourceBarrierDesc> mAllResGenerateBarrier;

		RHISinglePoolSingleCmdListPtr mBarrierCmd;

		LSharedPtr<RHIStagingBufferPool> mStagingBufferPool;
	public:
		DeviceResourceGenerateHelper();

		RHICmdList* FlushStaging();

		void CheckStageRefresh();

		RHIResourcePtr CreateBuffer(RHIHeapType memoryType, RHIBufferDesc& resDesc, void* initData = nullptr, size_t initDataSize = 0);
		RHIResourcePtr CreateTexture(RHIResDesc resDesc, void* initData = nullptr, size_t dataSize = 0, const RHISubResourceCopyDesc sourceCopyOffset = {});
		
		RHIResourcePtr CreateBufferByRhiMemory(const RHIBufferDesc& desc, RHIMemoryPtr targetMemory, size_t& memoryOffset);
		RHIResourcePtr CreateTextureByRhiMemory(const RHIResDesc& resDesc, RHIMemoryPtr targetMemory, size_t& memoryOffset);
	};
}

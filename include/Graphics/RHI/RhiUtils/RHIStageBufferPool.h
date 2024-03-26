#pragma once

#include "Graphics/RHI/RHIPch.h"
#include "Graphics/RHI/RHIResource.h"
#include "Graphics/RHI/RHIFence.h"
#include "Graphics/RHI/RHIDevice.h"
namespace luna::graphics
{

struct RHIStagingBuffer 
{
	size_t mResourceId;
	RHIResourcePtr mBufferData;
	size_t mBufferSize;
};

class RENDER_API RHIStagingBufferPool
{
	LMap<size_t, RHIStagingBuffer> mResourceWaitingCopy;

	LMap<size_t, LArray<RHIStagingBuffer>> mResourceCopying;

	RHIFencePtr mCopyFence;

	RHIRenderQueuePtr mRenderQueue;

	RHISinglePoolMultiCmdListPtr mCopyCommandList;

	size_t mResourcePoolCount;

	LQueue<size_t> mUnusedResourceId;

	bool isPoolWorking;
public:
	RHIStagingBufferPool();

	void UploadToDstBuffer(size_t dataLength, void* initData, RHIResource* dstBuffer,size_t offset_dst);

	void UploadToDstTexture(
		size_t dataLength,
		void* initData,
		const RHISubResourceCopyDesc& sourceCopyOffset,
		RHIResource* dstTexture,
		size_t offset_dst
	);

	void TickPoolRefresh();
private:
	RHIStagingBuffer* CreateUploadBuffer(size_t allocLength, size_t dataLength, void* initData);

	void UploadToDstResource(size_t dataLength, void* initData, RHIResource* dstTexture, size_t offset_dst, std::function<void(RHICmdList* curCmdList, RHIStagingBuffer* srcBuffer)> copyCommand);
};

}

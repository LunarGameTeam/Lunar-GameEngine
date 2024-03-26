#pragma once

#include "Graphics/RHI/RHIPch.h"
#include "Graphics/RHI/RHIResource.h"
#include "Graphics/RHI/RHIDescriptor.h"
#include "Graphics/RHI/RHIShader.h"
#include "Graphics/RHI/RhiUtils/RHIDeviceResourceHelper.h"
namespace luna::graphics
{
	class RhiUniformBufferPool;

	class RhiUniformBufferPack
	{
		RHIResource* mPhysicBuffer;

		size_t mOffset;

		size_t mSize;

		size_t mUid;

		RhiUniformBufferPool* mPool = nullptr;
	public:
		RhiUniformBufferPack();

		RhiUniformBufferPack(
			RhiUniformBufferPool* pool,
			RHIResource* physicBuffer,
			size_t offset,
			size_t size,
			size_t uid
		);

		~RhiUniformBufferPack();

		inline RHIResource* GetRhiBuffer()
		{
			return mPhysicBuffer;
		}

		inline size_t GetOffset()
		{
			return mOffset;
		}

		inline size_t GetSize()
		{
			return mSize;
		}

		inline size_t GetUid()
		{
			return mUid;
		}
	};

	class RhiUniformBufferPool
	{
		LArray<RHIResourcePtr> resourcePack;

		size_t mSizePerBuffer = 0;

		size_t mAllocOffset = 0;

		size_t mMaxUid = 0;

		LUnorderedMap<size_t, LUnorderedMap<size_t, RhiUniformBufferPack>> mEmptyPool;

		DeviceResourceGenerateHelper* mResCreateHelper;
	public:
		RhiUniformBufferPool(DeviceResourceGenerateHelper* resCreateHelper);

		RhiUniformBufferPack AllocUniform(size_t uniformSize);

		void CreateUniformBufferAndView(const RHICBufferDesc& uniform, RhiUniformBufferPack& bufferOut, RHIViewPtr& viewOut);

		void FreeUniform(RhiUniformBufferPack* uniformData);
	private:
		bool CheckHasEnoughSize(size_t sizeAlloc);

		void GenerateNewBuffer();
	};
}

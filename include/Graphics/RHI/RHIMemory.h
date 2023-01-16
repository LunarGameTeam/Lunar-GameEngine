#pragma once
#include "Graphics/RenderConfig.h"
#include "Graphics/RHI/RHITypes.h"
#include "Graphics/RHI/RHIPch.h"
#include "Graphics/RHI/RHIPtr.h"

namespace luna::render
{

struct RHIMemoryDesc
{
	RHIHeapType       Type = RHIHeapType::Default;
	uint64_t          SizeInBytes = 0;
	uint64_t          Alignment = 0;
	RHIHeapFlag       Flags = RHIHeapFlag::HEAP_FLAG_NONE;
};

class RENDER_API RHIMemory : public RHIObject
{
public:
	RHIMemoryDesc mMemoryDesc;

	RHIMemory(const RHIMemoryDesc& resource_desc)
	{
		mMemoryDesc = resource_desc;
	};

	const RHIMemoryDesc& GetDesc() const
	{
		return mMemoryDesc;
	}

	virtual  void* Map()
	{
		return nullptr;
	}

	virtual void Unmap()
	{

	}

	~RHIMemory() override
	{
	};
};
}

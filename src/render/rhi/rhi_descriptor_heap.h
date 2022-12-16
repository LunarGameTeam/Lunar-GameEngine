#pragma once

#include "render/render_config.h"
#include "render/rhi/rhi_pch.h"


namespace luna::render
{

struct DescriptorPoolDesc
{
	LUnorderedMap<DescriptorHeapType, uint32_t> mPoolAllocateSizes;
};

class RENDER_API RHIDescriptorPool : public RHIObject
{
public:

};
}

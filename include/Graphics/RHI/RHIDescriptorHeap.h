#pragma once

#include "Graphics/RenderConfig.h"
#include "Graphics/RHI/RHIPch.h"


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

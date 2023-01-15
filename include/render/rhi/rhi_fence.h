#pragma once
#include "render/rhi/rhi_pch.h"
#include "render/rhi/rhi_types.h"
#include "render/rhi/rhi_ptr.h"

//fence,用于检测gpu端命令是否运行完毕
namespace luna ::render
{

class RENDER_API RHIFence : public RHIObject
{
protected:
	size_t mValue = 1;
public:
	RHIFence() = default;
	virtual ~RHIFence() = default;


	virtual size_t IncSignal(RHIRenderQueue* queue) = 0;
	virtual uint64_t GetCompletedValue() = 0;
	virtual void Wait(size_t fence_value_check) = 0;
};
}

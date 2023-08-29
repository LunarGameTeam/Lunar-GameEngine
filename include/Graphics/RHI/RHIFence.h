#pragma once
#include "Graphics/RHI/RHIPch.h"
#include "Graphics/RHI/RHITypes.h"
#include "Graphics/RHI/RHIPtr.h"

//fence,用于检测gpu端命令是否运行完毕
namespace luna ::graphics
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

	size_t GetNextSignalValue() { return mValue; };
};
}

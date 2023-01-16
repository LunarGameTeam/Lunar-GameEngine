#pragma once
#include "Graphics/RenderConfig.h"
#include "Graphics/RHI/RHIFence.h"
#include "DX12Pch.h"


namespace luna::render
{
class RENDER_API DX12Fence : public RHIFence
{
	HANDLE mEvent;
public:
	Microsoft::WRL::ComPtr<ID3D12Fence> mFence;
	DX12Fence();
	size_t IncSignal(RHIRenderQueue* queue) override;
	uint64_t GetCompletedValue() override;
	void Wait(size_t fence_value_check) override;

	ID3D12Fence* GetFenceObject()
	{
		return mFence.Get();
	}

private:
	bool InitFence();
};
}

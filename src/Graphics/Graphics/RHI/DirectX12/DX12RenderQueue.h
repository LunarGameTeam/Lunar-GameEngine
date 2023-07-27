#pragma once
#include "Graphics/RenderConfig.h"
#include "DX12CommandList.h"


namespace luna::graphics
{
static constexpr int32_t MaxCommonListSubmit = 64;


class RENDER_API DX12RenderQueue : public RHIRenderQueue
{
	//渲染队列
public:
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> mDirectQueue;

	DX12RenderQueue(RHIQueueType type = RHIQueueType::eGraphic);
	~DX12RenderQueue();
	void ExecuteCommandLists(RHIGraphicCmdList* commond_list_array) override;

	RHISwapChainPtr CreateSwapChain(LWindow* window,const RHISwapchainDesc& windowDesc) override;

	void Wait(RHIFence* fence, uint64_t value) override;
	void Signal(RHIFence* fence, size_t fence_value) override;

	ID3D12CommandQueue* GetCommandQueueByPipelineType(const RHICmdListType& pipeline_type);


private:
};
}

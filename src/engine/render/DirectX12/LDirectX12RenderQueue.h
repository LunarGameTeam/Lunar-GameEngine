#pragma once
#include"LDirectX12CommondList.h"
namespace luna
{
	static constexpr int32_t MaxCommonListSubmit = 64;
	class ILunarDirectXGraphicRenderQueueCore :public ILunarGraphicRenderQueueCore
	{
		//渲染队列
		Microsoft::WRL::ComPtr<ID3D12CommandQueue> command_queue_direct;
		Microsoft::WRL::ComPtr<ID3D12CommandQueue> command_queue_copy;
		Microsoft::WRL::ComPtr<ID3D12CommandQueue> command_queue_compute;
	public:
		ILunarDirectXGraphicRenderQueueCore();
		~ILunarDirectXGraphicRenderQueueCore();
		void ExecuteCommandLists(const int32_t& commond_list_num, LVector<ILunarGraphicRenderCommondList*> commond_list_array) override;
		ILunarGraphicRenderSwapChain* CreateSwapChain(
			const void* trarget_window,
			const LWindowRenderDesc& trarget_window_desc,
			const LunarGraphicPipeLineType& pipeline_type
		) override;
	private:
		LResult InitRenderQueueData() override;
		ID3D12CommandQueue* GetCommandQueueByPipelineType(const LunarGraphicPipeLineType& pipeline_type);
	};
}

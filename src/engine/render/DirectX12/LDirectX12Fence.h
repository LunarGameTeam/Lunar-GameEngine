#pragma once
#include "LDirectXCommon.h"
namespace luna
{
	class LDx12GraphicRenderFence :public ILunarGraphicRenderFence
	{
		Microsoft::WRL::ComPtr<ID3D12Fence> directx_fence;
		HANDLE wait_thread_ID;
	public:
		LDx12GraphicRenderFence();
		size_t SetFence() override;
		bool CheckIfFenceFinished(const size_t& fence_value_check) override;
		void WaitForFence(const size_t& fence_value_check) override;
	private:
		LResult InitGraphicRenderFence() override;
	};
}
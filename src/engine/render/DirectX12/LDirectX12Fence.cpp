#include "LDirectX12Fence.h"
using namespace luna;
LDx12GraphicRenderFence::LDx12GraphicRenderFence():wait_thread_ID(nullptr)
{
}
size_t LDx12GraphicRenderFence::SetFence()
{
	HRESULT hr = directx_fence->Signal(fence_value_self_add);
	if (hr != S_OK)
	{
		LResult error_message;
		LunarDebugLogError(0, "signal to directx fence failed ", error_message);
	}
	fence_value_self_add += 1;
	return fence_value_self_add - 1;
}
bool LDx12GraphicRenderFence::CheckIfFenceFinished(const size_t& fence_value_check)
{
	size_t now_wait_value = directx_fence->GetCompletedValue();
	if (now_wait_value < fence_value_check)
	{
		return false;
	}
	return true;
}
void LDx12GraphicRenderFence::WaitForFence(const size_t& fence_value_check)
{
	if (!CheckIfFenceFinished(fence_value_check))
	{
		HRESULT hr;
		hr = directx_fence->SetEventOnCompletion(fence_value_check, wait_thread_ID);
		if (FAILED(hr))
		{
			LResult error_message;
			LunarDebugLogError(0, "reflect GPU thread fence to CPU thread handle failed ", error_message);
		}
		WaitForSingleObject(wait_thread_ID, INFINITE);
	}
}
LResult LDx12GraphicRenderFence::InitGraphicRenderFence()
{
	LResult check_error;
	//获取directx设备
	ID3D12Device* directx_device = reinterpret_cast<ID3D12Device*>(ILunarGraphicDeviceCore::GetInstance()->GetVirtualDevice());
	if (directx_device == nullptr)
	{
		LResult error_message;
		LunarDebugLogError(0, "Directx device is broken ", error_message);
		return error_message;
	}
	//创建用于接收gpu同步信息的fence
	HRESULT hr = directx_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&directx_fence));
	if (FAILED(hr))
	{
		LResult error_message;
		LunarDebugLogError(0, "Create Directx Fence Error ", error_message);
		return error_message;
	}
	//创建一个同步fence消息的事件
	wait_thread_ID = CreateEvent(nullptr, FALSE, FALSE, nullptr);
	if (wait_thread_ID == nullptr)
	{
		LResult error_message;
		LunarDebugLogError(0, "Create fence event Error When build commandlist", error_message);
		return error_message;
	}
	return g_Succeed;
}
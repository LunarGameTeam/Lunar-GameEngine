#include "LDirectX12ResourceHeap.h"
using namespace luna;
void luna::LDx12GraphicResourceHeap::CheckIfLoadingStateChanged(LLoadState& m_object_load_state)
{
	m_object_load_state = LLoadState::LOAD_STATE_FINISHED;
};
luna::LResult luna::LDx12GraphicResourceHeap::InitResorceByDesc(const D3D12_HEAP_DESC& resource_desc)
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
	HRESULT hr = directx_device->CreateHeap(&resource_desc, IID_PPV_ARGS(&resource_heap_data));
	if (FAILED(hr))
	{
		LResult error_message;
		LunarDebugLogError(E_FAIL, "Create resource heap error", error_message);
		return error_message;
	}
	return g_Succeed;
};
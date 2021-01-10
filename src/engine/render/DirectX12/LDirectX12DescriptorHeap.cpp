#include "LDirectX12DescriptorHeap.h"
using namespace luna;
void luna::LDx12GraphicDescriptorHeap::CheckIfLoadingStateChanged(LLoadState& m_object_load_state)
{
	m_object_load_state = LLoadState::LOAD_STATE_FINISHED;
};
luna::LResult luna::LDx12GraphicDescriptorHeap::InitResorceByDesc(const D3D12_DESCRIPTOR_HEAP_DESC& resource_desc)
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
	HRESULT hr = directx_device->CreateDescriptorHeap(&resource_desc, IID_PPV_ARGS(&descriptor_heap_data));
	if (FAILED(hr))
	{
		LResult error_message;
		LunarDebugLogError(E_FAIL, "Create descriptor heap error", error_message);
		return error_message;
	}
	return g_Succeed;
};
void luna::InitDescriptorHeapJsonReflect()
{
	InitNewStructToReflection(D3D12_DESCRIPTOR_HEAP_DESC);
}
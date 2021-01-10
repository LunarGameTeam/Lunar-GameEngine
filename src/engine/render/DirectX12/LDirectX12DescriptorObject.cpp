#include "LDirectX12DescriptorObject.h"
#include "LDirectX12DescriptorHeap.h"
#include "LDirectX12Buffer.h"
#include "LDirectX12Texture.h"
using namespace luna;
luna::LDx12DescriptorObject::LDx12DescriptorObject(
	const LunarGraphicDescriptorType& descriptor_type,
	const size_t& m_descriptor_offset,
	const size_t& descriptor_size) :
	ILunarGraphicRenderDescriptor(descriptor_type, m_descriptor_offset, descriptor_size),
	descriptor_cpu_handle_begin(),
	descriptor_gpu_handle_begin()
{
}
void luna::LDx12DescriptorObject::BindDirectxResourceToDescriptor(ID3D12Device* directx_device, ID3D12Resource* resource, const size_t& index_bind, LDx12CompleteDescriptorDesc* buffer_desc_pointer)
{
	D3D12_CPU_DESCRIPTOR_HANDLE now_bind_handle = descriptor_cpu_handle_begin;
	now_bind_handle.ptr += per_descriptor_size * index_bind;
	switch (m_descriptor_type)
	{
	case luna::DESCRIPTOR_UNIFORM_BUFFER:
	{
		D3D12_CONSTANT_BUFFER_VIEW_DESC uniform_buffer_desc;
		uniform_buffer_desc.BufferLocation = resource->GetGPUVirtualAddress() + buffer_desc_pointer->m_offset;
		uniform_buffer_desc.SizeInBytes = static_cast<UINT>(buffer_desc_pointer->m_block_size);
		directx_device->CreateConstantBufferView(&uniform_buffer_desc, now_bind_handle);
		break;
	}
	case luna::DESCRIPTOR_SHADER_RESOURCE:
	{
		directx_device->CreateShaderResourceView(resource, &buffer_desc_pointer->m_shader_resource_view_desc, now_bind_handle);
		break;
	}
	case luna::DESCRIPTOR_UNORDERED_ACCESS:
	{
		directx_device->CreateUnorderedAccessView(resource, NULL, &buffer_desc_pointer->m_unordered_access_view_desc, now_bind_handle);
		break;
	}
	case luna::DESCRIPTOR_RENDER_TARGET:
	{
		directx_device->CreateRenderTargetView(resource, &buffer_desc_pointer->m_render_target_view_desc, now_bind_handle);
		break;
	}
	case luna::DESCRIPTOR_DEPTH_STENCIL:
	{
		directx_device->CreateDepthStencilView(resource, &buffer_desc_pointer->m_depth_stencil_view_desc, now_bind_handle);
		break;
	}
	default:
	{
		LResult error_message;
		LunarDebugLogError(0, "unsurported descriptor type", error_message);
	}
	}
}
void luna::LDx12DescriptorObject::BindBufferToDescriptor(const LVector<LBasicAsset*>& buffer_data, const LVector<ILunarGraphicDescriptorCompleteDesc*>& buffer_descriptor_desc)
{
	//获取directx设备
	ID3D12Device* directx_device = reinterpret_cast<ID3D12Device*>(ILunarGraphicDeviceCore::GetInstance()->GetVirtualDevice());
	if (directx_device == nullptr)
	{
		LResult error_message;
		LunarDebugLogError(0, "Directx device is broken ", error_message);
	}
	//检查绑定资源数量
	if (buffer_data.size() > m_descriptor_size || buffer_data.size() != buffer_descriptor_desc.size())
	{
		LResult error_message;
		LunarDebugLogError(0, "required descriptor size too much or dismatch desc size", error_message);
	}
	//绑定资源
	for (size_t index_buffer_data = 0; index_buffer_data < buffer_data.size(); ++index_buffer_data)
	{
		LDx12GraphicResourceBuffer* buffer_data_pointer = dynamic_cast<LDx12GraphicResourceBuffer*>(buffer_data[index_buffer_data]);
		if (buffer_data_pointer == nullptr)
		{
			LResult error_message;
			LunarDebugLogError(0, "buffer data broken", error_message);
		}
		LDx12CompleteDescriptorDesc* buffer_desc_pointer = dynamic_cast<LDx12CompleteDescriptorDesc*>(buffer_descriptor_desc[index_buffer_data]);
		if (buffer_desc_pointer == nullptr)
		{
			LResult error_message;
			LunarDebugLogError(0, "buffer descriptor desc broken", error_message);
		}
		auto buffer_bind_type = buffer_desc_pointer->GetDesc();
		if (buffer_bind_type != m_descriptor_type)
		{
			LResult error_message;
			LunarDebugLogError(0, "buffer descriptor bind type dismatch", error_message);
		}
		BindDirectxResourceToDescriptor(directx_device, buffer_data_pointer->GetResource(), index_buffer_data, buffer_desc_pointer);
	}
}
void luna::LDx12DescriptorObject::BindTextureToDescriptor(const LVector<LBasicAsset*>& texture_data, const LVector<ILunarGraphicDescriptorCompleteDesc*>& tex_descriptor_desc)
{
	//获取directx设备
	ID3D12Device* directx_device = reinterpret_cast<ID3D12Device*>(ILunarGraphicDeviceCore::GetInstance()->GetVirtualDevice());
	if (directx_device == nullptr)
	{
		LResult error_message;
		LunarDebugLogError(0, "Directx device is broken ", error_message);
	}
	//检查绑定资源数量
	if (texture_data.size() > m_descriptor_size || tex_descriptor_desc.size() != tex_descriptor_desc.size())
	{
		LResult error_message;
		LunarDebugLogError(0, "required descriptor size too much or dismatch desc size", error_message);
	}
	//绑定资源
	for (size_t index_texture_data = 0; index_texture_data < texture_data.size(); ++index_texture_data)
	{
		LDx12GraphicResourceTexture* texture_data_pointer = dynamic_cast<LDx12GraphicResourceTexture*>(texture_data[index_texture_data]);
		if (texture_data_pointer == nullptr)
		{
			LResult error_message;
			LunarDebugLogError(0, "texture data broken", error_message);
		}
		LDx12CompleteDescriptorDesc*texture_desc_pointer = dynamic_cast<LDx12CompleteDescriptorDesc*>(texture_data[index_texture_data]);
		if (texture_desc_pointer == nullptr)
		{
			LResult error_message;
			LunarDebugLogError(0, "texture descriptor desc broken", error_message);
		}
		auto texture_bind_type = texture_desc_pointer->GetDesc();
		if (texture_bind_type != m_descriptor_type)
		{
			LResult error_message;
			LunarDebugLogError(0, "texture descriptor bind type dismatch", error_message);
		}
		BindDirectxResourceToDescriptor(directx_device, texture_data_pointer->GetResource(), index_texture_data, texture_desc_pointer);
	}
}
luna::LResult luna::LDx12DescriptorObject::InitGraphicRenderDescriptor(LObject* descriptor_heap)
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
	//根据描述符类型确定每个描述符所占字节数
	D3D12_DESCRIPTOR_HEAP_TYPE descriptor_type;
	check_error = GetDx12DescriptorType(descriptor_type);
	if (!check_error.m_IsOK)
	{
		return check_error;
	}
	per_descriptor_size = directx_device->GetDescriptorHandleIncrementSize(descriptor_type);
	//获取descriptorheap;
	LDx12GraphicDescriptorHeap* descriptor_heap_pointer = dynamic_cast<LDx12GraphicDescriptorHeap*>(descriptor_heap);
	if (descriptor_heap_pointer == nullptr)
	{
		LResult error_message;
		LunarDebugLogError(0, "descriptor heap is broken ", error_message);
		return error_message;
	}
	//检查描述符与描述符堆的格式是否匹配
	if (descriptor_heap_pointer->GetDesc().Type != descriptor_type)
	{
		LResult error_message;
		LunarDebugLogError(0, "descriptor type not match descriptor heap type ", error_message);
		return error_message;
	}
	//记录当前描述符的CPU指针位置以及GPU指针位置
	descriptor_cpu_handle_begin = descriptor_heap_pointer->GetResourceHeap()->GetCPUDescriptorHandleForHeapStart();
	descriptor_cpu_handle_begin.ptr += per_descriptor_size * m_descriptor_offset;
	descriptor_gpu_handle_begin = descriptor_heap_pointer->GetResourceHeap()->GetGPUDescriptorHandleForHeapStart();
	descriptor_gpu_handle_begin.ptr += per_descriptor_size * m_descriptor_offset;
	return g_Succeed;
}
 luna::LResult luna::LDx12DescriptorObject::GetDx12DescriptorType(D3D12_DESCRIPTOR_HEAP_TYPE &descriptor_type)
{
	switch (m_descriptor_type)
	{
	case luna::DESCRIPTOR_UNIFORM_BUFFER:
	{	
		descriptor_type = D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		break;
	}
	case luna::DESCRIPTOR_SHADER_RESOURCE:
	{
		descriptor_type = D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		break;
	}
	case luna::DESCRIPTOR_UNORDERED_ACCESS:
	{
		descriptor_type = D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		break;
	}
	case luna::DESCRIPTOR_RENDER_TARGET:
	{
		descriptor_type = D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		break;
	}
	case luna::DESCRIPTOR_DEPTH_STENCIL:
	{
		descriptor_type = D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
		break;
	}
	default:
	{
		LResult error_message;
		LunarDebugLogError(E_FAIL, "invalid descriptor type", error_message);
		return error_message;
		break;
	}
	}
	return g_Succeed;
}
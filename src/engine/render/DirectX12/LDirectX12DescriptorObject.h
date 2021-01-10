#pragma once
#include "LDirectXCommon.h"
namespace luna
{
	class LDx12CompleteDescriptorDesc : public ILunarGraphicDescriptorCompleteDesc
	{
	public:
		//存储所有的描述符格式
		size_t m_offset = 0;
		size_t m_block_size = 0;
		D3D12_SHADER_RESOURCE_VIEW_DESC m_shader_resource_view_desc = {};
		D3D12_UNORDERED_ACCESS_VIEW_DESC m_unordered_access_view_desc = {};
		D3D12_RENDER_TARGET_VIEW_DESC m_render_target_view_desc = {};
		D3D12_DEPTH_STENCIL_VIEW_DESC m_depth_stencil_view_desc = {};
		LDx12CompleteDescriptorDesc(const LunarGraphicDescriptorType& descriptor_type) :ILunarGraphicDescriptorCompleteDesc(descriptor_type) {};
	};
	class LDx12DescriptorObject :public ILunarGraphicRenderDescriptor
	{
		D3D12_CPU_DESCRIPTOR_HANDLE descriptor_cpu_handle_begin;
		D3D12_GPU_DESCRIPTOR_HANDLE descriptor_gpu_handle_begin;
		size_t per_descriptor_size;
	public:
		LDx12DescriptorObject(const LunarGraphicDescriptorType& descriptor_type, const size_t& m_descriptor_offset, const size_t& descriptor_size);
		void BindBufferToDescriptor(const LVector<LBasicAsset*> &buffer_data, const LVector<ILunarGraphicDescriptorCompleteDesc*>& buffer_descriptor_desc) override;
		void BindTextureToDescriptor(const LVector<LBasicAsset*> &texture_data, const LVector<ILunarGraphicDescriptorCompleteDesc*>& tex_descriptor_desc) override;
	private:
		LResult InitGraphicRenderDescriptor(LObject* descriptor_heap) override;
		LResult GetDx12DescriptorType(D3D12_DESCRIPTOR_HEAP_TYPE& descriptor_type);
		void BindDirectxResourceToDescriptor(ID3D12Device* directx_device,ID3D12Resource* resource,const size_t &index_bind, LDx12CompleteDescriptorDesc* buffer_desc_pointer);
	};
}
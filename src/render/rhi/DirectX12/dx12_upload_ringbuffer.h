#pragma once
#include "render/pch.h"
#include "dx12_rhi.h"


//纹理数据
namespace luna::render
{
//upload缓冲区资源块
struct UploadResBlock
{
	//存储开辟空间前后的指针位置，由于并非每次开辟都是首尾相连，所以开辟前的位置+存储区的大小并不一定等于开辟后的位置
	size_t pointer_before_alloc;
	size_t pointer_after_alloc;
	RHIResourcePtr m_dynamic_buffer_resource;
	TRHIPtr<RHIResource> m_static_gpu_resource;

	UploadResBlock(
		const size_t& pointer_before_alloc_in,
		const size_t& pointer_after_alloc_in,
		const uint64_t& memory_size_in,
		RHIResourcePtr& dynamic_buffer_resource,
		TRHIPtr<RHIResource>& static_gpu_resource_input
	)
	{
		pointer_before_alloc = pointer_before_alloc_in;
		pointer_after_alloc = pointer_after_alloc_in;
		m_static_gpu_resource = static_gpu_resource_input;
		m_dynamic_buffer_resource = dynamic_buffer_resource;
	}
};


class LDirectx12DynamicRingBuffer
{
public:
	LDirectx12DynamicRingBuffer();
	~LDirectx12DynamicRingBuffer();

	//拷贝数据到显存
	UploadResBlock* AllocResUploadBlock(const void* data, size_t size, TRHIPtr<RHIResource>& gpu_res);

	void CopyDataToGpu(
		const void* data_pointer,
		size_t data_size_in,
		TRHIPtr<RHIResource>& gpu_resource_pointer
	);

	void CopyDataToGpu(
		const void* data_pointer,
		size_t data_size_in,
		TRHIPtr<RHIResource>& gpu_resource_pointer,
		uint32_t src_subresource_index,
		uint32_t dst_subresource_index
	);

	void CopyDataToGpu(
		const void* data_pointer,
		size_t data_size_in,
		TRHIPtr<RHIResource>& gpu_resource_pointer,
		const RHIPlacedSubResFootprint& src_layout,
		uint32_t dst_subresource_index
	);

	void CopyDataToGpu(
		LVector<D3D12_SUBRESOURCE_DATA>& subresources,
		LVector<D3D12_PLACED_SUBRESOURCE_FOOTPRINT>& src_layout,
		LVector<UINT64> pRowSizesInBytes,
		LVector<UINT> pNumRows,
		TRHIPtr<RHIResource>& gpu_resource_pointer,
		size_t data_size_in);

private:
	UploadResBlock* _InternalAllocBlock(
		size_t data_size,
		TRHIPtr<RHIResource>& gpu_resource_pointer		
	);

	//加载初始化信息
	void LoadInitData();

	//刷新老的缓冲区，释放不需要的资源所占的空间
	void RefreshOldDynamicData();

	//标准拷贝命令
	void CopyToGPU(
		RHIResource* src_dynamic_buffer_pointer,
		RHIResource* dst_static_buffer_pointer,
		std::function<void()> deal_func
	);

	//buffer拷贝到buffer
	void _CopyFromDynamicBufferToGpu(
		RHIResource* src,
		RHIResource* dst,
		size_t src_offset,
		size_t dst_offset,
		size_t data_size
	);

	//buffer拷贝到texture
	void _CopyFromDynamicBufferToGpu(
		RHIResource* src,
		RHIResource* dst,
		const RHIPlacedSubResFootprint& src_layout,
		uint32_t dst_subresource_index
	);

	//buffer拷贝到texture的各个subresource
	void _CopyFromDynamicBufferToGpu(
		RHIResource* src,
		RHIResource* dst,
		const LVector<RHIPlacedSubResFootprint>& src_layout
	);

	//texture拷贝到texture
	void _CopyFromDynamicTextureToGpu(
		RHIResource* src_dynamic_buffer_pointer,
		RHIResource* dst_static_buffer_pointer,
		uint32_t src_subresource_index,
		uint32_t dst_subresource_index
	);

private:
	TRHIPtr<RHIMemory> ringbuffer_heap_data;
	size_t buffer_size;
	size_t pointer_head_offset;
	size_t pointer_tail_offset;
	std::queue<UploadResBlock*> ResourceUploadingMap;	
	TRHIPtr<RHIGraphicCmdList> mTransferCmdList;
	TRHIPtr<RHIFence> mIncFence;
	bool mFirstCopy = true;
};
}

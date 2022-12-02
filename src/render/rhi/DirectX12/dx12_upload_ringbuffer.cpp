#include "dx12_device.h"
#include "dx12_upload_ringbuffer.h"
#include "dx12_resource.h"

#include "render/render_module.h"
#include "render/rhi/DirectX12//dx12_command_list.h"

using Microsoft::WRL::ComPtr;


namespace luna::render
{
//中间动态资源的分配池
LDirectx12DynamicRingBuffer::LDirectx12DynamicRingBuffer()
{
	LoadInitData();
}

void LDirectx12DynamicRingBuffer::LoadInitData()
{
	buffer_size = 256 * 1024 * 1024;
	pointer_head_offset = 0;
	pointer_tail_offset = 0;
	RHIMemoryDesc desc;
	desc.Alignment = 0;
	desc.Flags = RHIHeapFlag::HEAP_FLAG_ALLOW_ONLY_BUFFERS;
	desc.Type = RHIHeapType::Upload;
	desc.SizeInBytes = buffer_size;
	ringbuffer_heap_data = sRenderModule->GetDevice<DX12Device>()->AllocMemory(desc);
	//注册拷贝队列 = sRenderModule->GetDevice<DX12Device>()->CreateCommondAllocator(
	//	RHICmdListType::Copy);

	mTransferCmdList = sRenderModule->GetDevice<DX12Device>()->CreateCommondList(RHICmdListType::Graphic3D);
	//创建fence
	mIncFence = sRenderModule->GetDevice<DX12Device>()->CreateFence();
}

LDirectx12DynamicRingBuffer::~LDirectx12DynamicRingBuffer()
{
	while (!ResourceUploadingMap.empty())
	{
		auto top_data = ResourceUploadingMap.front();
		delete top_data;
		ResourceUploadingMap.pop();
	}
}

UploadResBlock* LDirectx12DynamicRingBuffer::_InternalAllocBlock(
	size_t size,
	TRHIPtr<RHIResource>& gpu_res
)
{
	//如果ring-buffer中还存在待处理的信息，则运行一遍资源清理函数
	if (!ResourceUploadingMap.empty())
	{
		RefreshOldDynamicData();
	}
	size_t alloc_start_position;
	//查看当前缓冲区是否有足够的空间
	if (pointer_tail_offset > pointer_head_offset)
	{
		//头指针小于尾指针，此时的可用空间仅有头尾指针之间的空间
		size_t head_could_use_size = pointer_tail_offset - pointer_head_offset;
		if (head_could_use_size < size)
		{
			return nullptr;
		}
		alloc_start_position = pointer_head_offset;
	}
	else
	{
		//头指针大于尾指针，此时的可用空间有两片，其一是头指针到buffer尾部的空间，其二是buffer头到尾指针的空间
		size_t head_could_use_size_1 = buffer_size - pointer_head_offset;
		if (head_could_use_size_1 >= size)
		{
			alloc_start_position = pointer_head_offset;
		}
		else
		{
			size_t head_could_use_size_2 = pointer_tail_offset;
			if (head_could_use_size_2 >= size)
			{
				alloc_start_position = 0;
			}
			else
			{
				
				LogError("Device", "dynamic ring-buffer is full, could not alloc new data");
				return nullptr;
			}
		}
	}
	//根据新的头部指针信息，开辟buffer数据
	RHIResourcePtr resource_data = sRenderModule->GetDevice<DX12Device>()->CreatePlacedBuffer(
		ringbuffer_heap_data.get(), alloc_start_position, size, LResState::LUNA_RESOURCE_STATE_GENERIC_READ);
	//将创建成功的资源绑定到gpu资源
	auto res = new UploadResBlock(
		pointer_head_offset,
		alloc_start_position + size,
		size,
		resource_data,
		gpu_res
	);
	pointer_head_offset = alloc_start_position + size;
	return res;
}

void LDirectx12DynamicRingBuffer::CopyToGPU(
	RHIResource* src_res,
	RHIResource* dst_res,
	std::function<void()> deal_func
)
{
	DX12Resource* dx_src_res = src_res->As<DX12Resource>();
	DX12Resource* dx_dst_res = dst_res->As<DX12Resource>();
	LDxResourceBlockLoadState state = dx_dst_res->mResBlock->GetResourceLoadingState();

	assert(state != RESOURCE_LOAD_FAILED);
	assert(dx_src_res->mResBlock->GetResourceLoadingState() == RESOURCE_LOAD_FINISH);
	assert(state == RESOURCE_LOAD_FINISH);
	
	if (!mFirstCopy)
	{
		mTransferCmdList->Reset();
	}

	mTransferCmdList->BeginEvent("Transfer");
	mTransferCmdList->ResourceBarrier(dst_res, LResState::LUNA_RESOURCE_STATE_COPY_DEST);
	deal_func();
	mTransferCmdList->ResourceBarrier(dst_res, LResState::LUNA_RESOURCE_STATE_COMMON);
	mTransferCmdList->EndEvent();
	mTransferCmdList->CloseCommondList();
	sRenderModule->GetCmdQueueCore()->ExecuteCommandLists(mTransferCmdList);
	mFirstCopy = false;
	dx_dst_res->mResBlock->SetResourceLoadingState(RESOURCE_LOAD_GPU_LOADING);
}

void LDirectx12DynamicRingBuffer::_CopyFromDynamicBufferToGpu(
	RHIResource* src_res,
	RHIResource* dst_res,
	size_t src_offset,
	size_t dst_offset,
	size_t data_size
)
{
	CopyToGPU(
		src_res,
		dst_res,
		[&]()
		{
			DX12Resource* src_dynamic_buffer = src_res->As<DX12Resource>();
			DX12Resource* dst_static_buffer = dst_res->As<DX12Resource>();
			auto src_size_check = src_offset + data_size;
			auto dst_size_check = dst_offset + data_size;
			if (src_size_check > src_dynamic_buffer->mResBlock->GetSize() || dst_size_check >
				dst_static_buffer->mResBlock->GetSize())
			{
				
				LogError("Device", "resource size overflow, could not copy resource from cpu to GPU");
				return;
			}
			mTransferCmdList.get()->CopyBufferToBuffer(
				dst_res,
				dst_offset,
				src_res,
				src_offset,
				data_size
			);
		}
	);
}

void LDirectx12DynamicRingBuffer::_CopyFromDynamicTextureToGpu(
	RHIResource* src_dst,
	RHIResource* dst_res,
	uint32_t src_sub_index,
	uint32_t dst_sub_index
)
{
	CopyToGPU(
		src_dst,
		dst_res,
		[&]()
		{
			mTransferCmdList.get()->CopyBufferToTexture(
				dst_res,
				dst_sub_index,
				src_dst,
				src_sub_index
			);
		}
	);
}

void LDirectx12DynamicRingBuffer::_CopyFromDynamicBufferToGpu(
	RHIResource* src_res,
	RHIResource* dst_res,
	const RHIPlacedSubResFootprint& src_layout,
	uint32_t dst_sub_index
)
{
	CopyToGPU(
		src_res,
		dst_res,
		[&]()
		{
			mTransferCmdList.get()->CopyTextureRegionByFootprint(
				dst_res,
				dst_sub_index,
				src_res,
				src_layout
			);
		}
	);
}

void LDirectx12DynamicRingBuffer::_CopyFromDynamicBufferToGpu(
	RHIResource* src_res,
	RHIResource* dst_res,
	const LVector<RHIPlacedSubResFootprint>& src_layout
)
{
	CopyToGPU(
		src_res,
		dst_res,
		[&]()
		{
			for (UINT i = 0; i < src_layout.size(); ++i)
			{
				D3D12_PLACED_SUBRESOURCE_FOOTPRINT real_layout = GetFootPrint(src_layout[i]);
				mTransferCmdList.get()->CopyTextureRegionByFootprint(
					dst_res,
					i,
					src_res,
					src_layout[i]
				);
			}
		}
	);
}


UploadResBlock* LDirectx12DynamicRingBuffer::AllocResUploadBlock(const void* data, size_t data_size, TRHIPtr<RHIResource>& gpu_res)
{

	//buffer数据需要64k对齐
	size_t dynamic_data_size = SizeAligned2Pow(data_size, 65536);
	UploadResBlock* block = _InternalAllocBlock(dynamic_data_size, gpu_res);
	assert(block != nullptr);
	//将数据从CPU拷贝到dynamic-buffer	
	if (data)
	{
		DX12Resource* dx12_pointer = block->m_dynamic_buffer_resource->As<DX12Resource>();
		dx12_pointer->WriteFromCpuToBuffer(0, data, data_size);
	}
	
	//将数据从dynamic-buffer拷贝到显存
	//添加资源记录到队列
	ResourceUploadingMap.push(block);
	return block;
}

void LDirectx12DynamicRingBuffer::CopyDataToGpu(
	const void* data,
	size_t data_size,
	TRHIPtr<RHIResource>& gpu_res
)
{
	UploadResBlock* new_dynamic_block = AllocResUploadBlock(
		data,
		data_size,
		gpu_res
	);
	_CopyFromDynamicBufferToGpu(
		new_dynamic_block->m_dynamic_buffer_resource.get(),
		gpu_res.get(),
		0,
		0,
		data_size
	);
}

void LDirectx12DynamicRingBuffer::CopyDataToGpu(
	const void* data,
	size_t data_size,
	TRHIPtr<RHIResource>& gpu_res,
	uint32_t src_sub_index,
	uint32_t dst_sub_index
)
{
	UploadResBlock* new_dynamic_block = AllocResUploadBlock(
		data,
		data_size,
		gpu_res
	);
	_CopyFromDynamicTextureToGpu(
		new_dynamic_block->m_dynamic_buffer_resource.get(),
		gpu_res.get(),
		src_sub_index,
		dst_sub_index
	);
}

void LDirectx12DynamicRingBuffer::CopyDataToGpu(
	const void* data,
	size_t data_size_in,
	TRHIPtr<RHIResource>& gpu_res,
	const RHIPlacedSubResFootprint& src_layout,
	uint32_t dst_sub_index
)
{
	UploadResBlock* new_dynamic_block = AllocResUploadBlock(
		data,
		data_size_in,
		gpu_res);
	_CopyFromDynamicBufferToGpu(
		new_dynamic_block->m_dynamic_buffer_resource.get(),
		gpu_res.get(),
		src_layout,
		dst_sub_index
	);
}

void LDirectx12DynamicRingBuffer::CopyDataToGpu(
	LVector<D3D12_SUBRESOURCE_DATA>& sub_res,
	LVector<D3D12_PLACED_SUBRESOURCE_FOOTPRINT>& footprints,
	LVector<UINT64> pRowSizesInBytes,
	LVector<UINT> pNumRows,
	TRHIPtr<RHIResource>& gpu_res,
	size_t size
)
{
	UploadResBlock* block = AllocResUploadBlock(nullptr, size, gpu_res);

	LVector<RHIPlacedSubResFootprint> pLayouts;
	for (const D3D12_PLACED_SUBRESOURCE_FOOTPRINT& it : footprints)
	{
		pLayouts.emplace_back(GetFootPrintDx(it));
	}
	DX12Resource* dx12_pointer = block->m_dynamic_buffer_resource->As<DX12Resource>();

	dx12_pointer->mResBlock->WriteFromCpuToBuffer(0,
		sub_res,
		footprints,
		pRowSizesInBytes,
		pNumRows);

	//将数据从dynamic-buffer拷贝到显存
	_CopyFromDynamicBufferToGpu(
		block->m_dynamic_buffer_resource.get(),
		gpu_res.get(),
		pLayouts
	);
}

void LDirectx12DynamicRingBuffer::RefreshOldDynamicData()
{
	while (!ResourceUploadingMap.empty())
	{
		auto top_data = ResourceUploadingMap.front();
		DX12Resource* src_dynamic_buffer = top_data->m_static_gpu_resource->As<DX12Resource>();

		if (src_dynamic_buffer->mResBlock->GetResourceLoadingState() == RESOURCE_LOAD_FINISH)
		{
			ResourceUploadingMap.pop();
			//检测当前资源释放之前的指针是否与期待的一致
			assert(top_data->pointer_before_alloc == pointer_tail_offset);
			//释放完毕后修改尾指针
			pointer_tail_offset = top_data->pointer_after_alloc;
			delete top_data;
		}
		else
		{
			break;
		}
	}
}
}

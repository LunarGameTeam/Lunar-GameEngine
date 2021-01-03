#pragma once
#include "LDirectXCommon.h"
namespace luna
{
	//Directx12资源加载状态
	enum LDxResourceBlockLoadState
	{
		RESOURCE_LOAD_FAILED = 0,
		RESOURCE_LOAD_GPU_LOADING,
		RESOURCE_LOAD_FINISH
	};
	//Directx12资源块
	class LDirectx12ResourceBlock
	{
		bool if_start_copying_gpu;
		luna::LSafeIndexType::LGraphicFenceId  wait_fence; //当前资源的加载等待信号量(仅用于显存资源)
		LDxResourceBlockLoadState  now_res_load_state;     //当前资源的加载状态
		size_t                     memory_size;            //存储块的大小
		Microsoft::WRL::ComPtr<ID3D12Resource>     resource_data;          //存储块的数据
		D3D12_HEAP_TYPE            resource_usage;
		UINT8* map_pointer;
		D3D12_RESOURCE_STATES      now_subresource_state; //当前资源的使用格式
	public:
		LDirectx12ResourceBlock(
			const uint64_t& memory_size_in,
			Microsoft::WRL::ComPtr<ID3D12Resource> resource_data_in,
			const D3D12_HEAP_TYPE& resource_usage_in,
			const D3D12_RESOURCE_STATES& resource_state
		);
		~LDirectx12ResourceBlock();
		inline ID3D12Resource* GetResource() const
		{
			return resource_data.Get();
		}
		inline uint64_t GetSize() const
		{
			return memory_size;
		}
		//查看当前资源的加载状态
		LDxResourceBlockLoadState GetResourceLoadingState();
		//等待GPU加载资源结束
		luna::LResult WaitForResourceLoading();
		//查看当前资源的使用格式
		D3D12_RESOURCE_STATES GetResourceState()
		{
			return now_subresource_state;
		}
		luna::LResult WriteFromCpuToBuffer(
			const size_t& pointer_offset,
			const void* copy_data,
			const size_t& data_size
		);
		luna::LResult WriteFromCpuToBuffer(
			const size_t& pointer_offset,
			std::vector<D3D12_SUBRESOURCE_DATA>& subresources,
			D3D12_PLACED_SUBRESOURCE_FOOTPRINT* pLayouts,
			UINT64* pRowSizesInBytes,
			UINT* pNumRows
		);
		luna::LResult ReadFromBufferToCpu(
			const size_t& pointer_offset,
			void* copy_data,
			const size_t data_size
		);
		luna::LResult SetResourceCopyBrokenFence(const luna::LSafeIndexType::LGraphicFenceId& broken_fence_id);
		luna::LResult GetCpuMapPointer(UINT8** map_pointer_out);
		/*
		luna::LResult CopyFromDynamicBufferToGpu(
			PancyRenderCommandList* commandlist,
			ResourceBlockGpu& dynamic_buffer,
			const size_t& src_offset,
			const size_t& dst_offset,
			const size_t& data_size
		);
		luna::LResult CopyFromDynamicBufferToGpu(
			PancyRenderCommandList* commandlist,
			ResourceBlockGpu& dynamic_buffer,
			D3D12_PLACED_SUBRESOURCE_FOOTPRINT* pLayouts,
			const LunarObjectID& Layout_num
		);
		//todo:不要手动调用，资源使用前进行检测，如果不一致，自动调用，不要传入两个参数
		luna::LResult ResourceBarrier(
			PancyRenderCommandList* commandlist,
			const D3D12_RESOURCE_STATES& next_state
		);
		*/
	private:
	};
}
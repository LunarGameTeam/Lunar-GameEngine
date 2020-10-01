#pragma once
#include"PancyDx12Basic.h"
#include"PancyThreadBasic.h"

//描述符类型
enum PancyDescriptorType
{
	DescriptorTypeShaderResourceView = 0,
	DescriptorTypeConstantBufferView,
	DescriptorTypeUnorderedAccessView,
	DescriptorTypeRenderTargetView,
	DescriptorTypeDepthStencilView,
};
struct BasicDescriptorDesc
{
	//描述符的类型
	PancyDescriptorType basic_descriptor_type;
	//存储所有的描述符格式
	LunarResourceSize offset = 0;
	LunarResourceSize block_size = 0;
	D3D12_SHADER_RESOURCE_VIEW_DESC shader_resource_view_desc = {};
	D3D12_UNORDERED_ACCESS_VIEW_DESC unordered_access_view_desc = {};
	D3D12_RENDER_TARGET_VIEW_DESC render_target_view_desc = {};
	D3D12_DEPTH_STENCIL_VIEW_DESC depth_stencil_view_desc = {};
};
//资源加载状态
enum PancyResourceLoadState
{
	RESOURCE_LOAD_FAILED = 0,
	RESOURCE_LOAD_CPU_FINISH,
	RESOURCE_LOAD_GPU_LOADING,
	RESOURCE_LOAD_GPU_FINISH
};
//GPU资源块
class ResourceBlockGpu
{
	bool if_start_copying_gpu;
	PancyFenceIdGPU  wait_fence;               //当前资源的加载等待信号量(仅用于显存资源)
	PancyResourceLoadState now_res_load_state; //当前资源的加载状态
	LunarResourceSize memory_size;           //存储块的大小
	ComPtr<ID3D12Resource> resource_data;      //存储块的数据
	D3D12_HEAP_TYPE resource_usage;
	UINT8* map_pointer;
	D3D12_RESOURCE_STATES now_subresource_state;//当前资源的使用格式
public:
	ResourceBlockGpu(
		const uint64_t& memory_size_in,
		ComPtr<ID3D12Resource> resource_data_in,
		const D3D12_HEAP_TYPE& resource_usage_in,
		const D3D12_RESOURCE_STATES& resource_state
	);
	~ResourceBlockGpu();
	inline ID3D12Resource* GetResource() const
	{
		return resource_data.Get();
	}
	inline uint64_t GetSize() const
	{
		return memory_size;
	}
	//创建资源访问格式
	LunarEngine::LResult BuildCommonDescriptorView(
		const BasicDescriptorDesc& descriptor_desc,
		const D3D12_CPU_DESCRIPTOR_HANDLE& DestDescriptor,
		bool if_wait_for_gpu = true
	);
	LunarEngine::LResult BuildVertexBufferView(
		const LunarResourceSize& offset,
		const LunarResourceSize& buffer_size,
		UINT StrideInBytes,
		D3D12_VERTEX_BUFFER_VIEW& VBV_out,
		bool if_wait_for_gpu = true
	);
	LunarEngine::LResult BuildIndexBufferView(
		const LunarResourceSize& offset,
		const LunarResourceSize& buffer_size,
		DXGI_FORMAT StrideInBytes,
		D3D12_INDEX_BUFFER_VIEW& IBV_out,
		bool if_wait_for_gpu = true
	);
	//查看当前资源的加载状态
	PancyResourceLoadState GetResourceLoadingState();
	//等待GPU加载资源结束
	LunarEngine::LResult WaitForResourceLoading();
	//查看当前资源的使用格式
	D3D12_RESOURCE_STATES GetResourceState()
	{
		return now_subresource_state;
	}
	LunarEngine::LResult WriteFromCpuToBuffer(
		const LunarResourceSize& pointer_offset,
		const void* copy_data,
		const LunarResourceSize& data_size
	);
	LunarEngine::LResult WriteFromCpuToBuffer(
		const LunarResourceSize& pointer_offset,
		std::vector<D3D12_SUBRESOURCE_DATA>& subresources,
		D3D12_PLACED_SUBRESOURCE_FOOTPRINT* pLayouts,
		UINT64* pRowSizesInBytes,
		UINT* pNumRows
	);
	LunarEngine::LResult CopyFromDynamicBufferToGpu(
		PancyRenderCommandList* commandlist,
		ResourceBlockGpu& dynamic_buffer,
		const LunarResourceSize& src_offset,
		const LunarResourceSize& dst_offset,
		const LunarResourceSize& data_size
	);
	LunarEngine::LResult CopyFromDynamicBufferToGpu(
		PancyRenderCommandList* commandlist,
		ResourceBlockGpu& dynamic_buffer,
		D3D12_PLACED_SUBRESOURCE_FOOTPRINT* pLayouts,
		const LunarObjectID& Layout_num
	);
	LunarEngine::LResult SetResourceCopyBrokenFence(const PancyFenceIdGPU& broken_fence_id);
	LunarEngine::LResult GetCpuMapPointer(UINT8** map_pointer_out)
	{
		if (now_res_load_state == RESOURCE_LOAD_FAILED)
		{
			LunarEngine::LResult error_message;
			LunarDebugLogError(E_FAIL, "resource load failed, could not copy data to memory", error_message);

			return error_message;
		}
		if (resource_usage != D3D12_HEAP_TYPE::D3D12_HEAP_TYPE_UPLOAD)
		{
			LunarEngine::LResult error_message;
			LunarDebugLogError(E_FAIL, "resource type is not upload, could not copy data to memory", error_message);

			map_pointer_out = NULL;
			return error_message;
		}
		*map_pointer_out = map_pointer;
		return LunarEngine::g_Succeed;
	}
	LunarEngine::LResult ReadFromBufferToCpu(
		const LunarResourceSize& pointer_offset,
		void* copy_data,
		const LunarResourceSize data_size
	);
	//todo:不要手动调用，资源使用前进行检测，如果不一致，自动调用，不要传入两个参数
	LunarEngine::LResult ResourceBarrier(
		PancyRenderCommandList* commandlist,
		const D3D12_RESOURCE_STATES& last_state,
		const D3D12_RESOURCE_STATES& now_state
	);
private:
	void BuildConstantBufferView(
		const LunarResourceSize& offset,
		const LunarResourceSize& block_size,
		const D3D12_CPU_DESCRIPTOR_HANDLE& DestDescriptor
	);
	void BuildShaderResourceView(
		const D3D12_CPU_DESCRIPTOR_HANDLE& DestDescriptor,
		const D3D12_SHADER_RESOURCE_VIEW_DESC& SRV_desc
	);
	void BuildRenderTargetView(
		const D3D12_CPU_DESCRIPTOR_HANDLE& DestDescriptor,
		const D3D12_RENDER_TARGET_VIEW_DESC& RTV_desc
	);
	void BuildUnorderedAccessView(
		const D3D12_CPU_DESCRIPTOR_HANDLE& DestDescriptor,
		const D3D12_UNORDERED_ACCESS_VIEW_DESC& UAV_desc
	);
	void BuildDepthStencilView(
		const D3D12_CPU_DESCRIPTOR_HANDLE& DestDescriptor,
		const D3D12_DEPTH_STENCIL_VIEW_DESC& DSV_desc
	);
};



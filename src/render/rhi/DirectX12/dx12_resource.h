#pragma once
#include "render/pch.h"
#include "dx12_rhi.h"

#include <DirectXTex.h>
#include <wincodec.h>
//#include <LoaderHelpers.h>
#include <DDSTextureLoader.h>
#include <WICTextureLoader.h>


//纹理数据
namespace DirectXTextureLoader
{
uint32_t RENDER_API MyCountMips(uint32_t width, uint32_t height);
inline bool RENDER_API CheckIfDepthStencil(DXGI_FORMAT fmt);
inline void RENDER_API MyAdjustPlaneResource(
	_In_ DXGI_FORMAT fmt,
	_In_ size_t height,
	_In_ size_t slicePlane,
	_Inout_ D3D12_SUBRESOURCE_DATA& res);
HRESULT MyFillInitData(_In_ size_t width,
                       _In_ size_t height,
                       _In_ size_t depth,
                       _In_ size_t mipCount,
                       _In_ size_t arraySize,
                       _In_ size_t numberOfPlanes,
                       _In_ DXGI_FORMAT format,
                       _In_ size_t maxsize,
                       _In_ size_t bitSize,
                       _In_reads_bytes_(bitSize) const uint8_t* bitData,
                       _Out_ size_t& twidth,
                       _Out_ size_t& theight,
                       _Out_ size_t& tdepth,
                       _Out_ size_t& skipMip,
                       std::vector<D3D12_SUBRESOURCE_DATA>& initData);
}


namespace luna::render
{
//Directx12资源加载状态
enum RENDER_API LDxResourceBlockLoadState
{
	RESOURCE_LOAD_FAILED = 0,
	RESOURCE_LOAD_GPU_LOADING,
	RESOURCE_LOAD_FINISH
};


class RENDER_API LDirectx12ResourceBlock
{
	bool mIfStartCopyingGpu;
	//luna::LSafeIndexType::LGraphicFenceId  wait_fence; //当前资源的加载等待信号量(仅用于显存资源)
	LDxResourceBlockLoadState mResLoadState; //当前资源的加载状态
	size_t memory_size; //存储块的大小
	Microsoft::WRL::ComPtr<ID3D12Resource> mResData; //存储块的数据
	D3D12_HEAP_TYPE mHeapType;
	UINT8* mMapPtr;
	D3D12_RESOURCE_STATES mNowResState; //当前资源的使用格式
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
		return mResData.Get();
	}

	inline uint64_t GetSize() const
	{
		return memory_size;
	}

	//查看当前资源的加载状态
	LDxResourceBlockLoadState GetResourceLoadingState();

	void SetResourceLoadingState(const LDxResourceBlockLoadState& load_state)
	{
		mResLoadState = load_state;
	}

	//等待GPU加载资源结束
	bool WaitForResourceLoading();
	//查看当前资源的使用格式
	D3D12_RESOURCE_STATES GetResourceState()
	{
		return mNowResState;
	}

	void SetResourceState(const D3D12_RESOURCE_STATES& res_state)
	{
		mNowResState = res_state;
	}

	void WriteFromCpuToBuffer(
		const size_t& pointer_offset,
		const void* copy_data,
		const size_t& data_size
	);

	void WriteFromCpuToBuffer(
		const size_t& pointer_offset,
		LVector<D3D12_SUBRESOURCE_DATA>& subresources,
		LVector<D3D12_PLACED_SUBRESOURCE_FOOTPRINT>& pLayouts,
		LVector<UINT64>& pRowSizesInBytes,
		LVector<UINT>& pNumRows);

	bool ReadFromBufferToCpu(
		const size_t& pointer_offset,
		void* copy_data,
		const size_t data_size
	);
	bool GetCpuMapPointer(UINT8** map_pointer_out);
private:
};

struct DxResourceCopyLayout
{
	std::vector<D3D12_PLACED_SUBRESOURCE_FOOTPRINT> pLayouts;
	std::vector<UINT> pNumRows;
	std::vector<UINT64> pRowSizeInBytes;
	UINT64 pTotalBytes = 0;
};


class RENDER_API DX12ResourceNew : public RHIResource
{
public:
	DX12ResourceNew() {}


	void UpdateUploadBuffer(size_t offset, const void* copy_data, size_t data_size) override {};

	void BindMemory(RHIMemory* memory, uint64_t offset) override;

	void RefreshMemoryRequirements();

	void* Map() override 
	{
		return mMapPointer;
	}
	void Unmap() override
	{
		/*do nothing*/
	}
	Microsoft::WRL::ComPtr<ID3D12Resource> mDxRes;
	void* mMapPointer = nullptr;
	D3D12_RESOURCE_DESC mDxDesc = {};
	D3D12_SAMPLER_DESC mDxSamplerDesc = {};
};

class RENDER_API DX12Resource : public RHIResource
{	
	size_t mSubResSize = 0;
	size_t mSubResNum = 0;
public:
	LDirectx12ResourceBlock* mResBlock = nullptr;
	DX12Resource(
		const RHIMemory* res_heap,
		const size_t offset,
		const RHIResDesc& resource_desc,
		const D3D12_RESOURCE_STATES& resource_build_state
	);


	DX12Resource(const RHIResDesc& resource_desc, 
		const size_t& subresources_num,
		const D3D12_RESOURCE_STATES& resource_build_state);

	DX12Resource(
		const RHIMemory* res_heap,
		const size_t offset,
		const RHIResDesc& resource_desc,
		const size_t& subresources_num,
		const D3D12_RESOURCE_STATES& resource_build_state
	);

	DX12Resource(const RHIResDesc& resource_desc,
		const D3D12_RESOURCE_STATES& resource_build_state);
	DX12Resource(
		const int32_t back_buffer_id,
		RHISwapChain* res_swapchain
	);

	void UpdateUploadBuffer(
		size_t offset,
		const void* copy_data,
		size_t data_size
	) override
	{
		WriteFromCpuToBuffer(offset, copy_data, data_size);
	};

	~DX12Resource()
	{
		delete mResBlock;
	}

	RHIResDesc& GetDesc()
	{
		return mResDesc;
	}

	inline ID3D12Resource* GetResource() const
	{
		if (mResBlock != nullptr)
		{
			return mResBlock->GetResource();
		}
		return nullptr;
	}


	void WriteFromCpuToBuffer(
		const size_t& pointer_offset,
		const void* copy_data,
		const size_t& data_size
	);

	void* Map() override;


	void Unmap() override;

private:
	bool InitResorceByDesc(
		const RHIResDesc& buffer_desc,
		const D3D12_RESOURCE_STATES& resource_build_state
	);
	bool InitResorceByDesc(
		const RHIMemory* res_heap,
		const size_t offset,
		const RHIResDesc& buffer_desc,
		const D3D12_RESOURCE_STATES& resource_build_state
	);
	bool _InitResorceByDesc(
		const RHIResDesc& buffer_desc,
		const D3D12_RESOURCE_STATES& resource_build_state,
		std::function<void(
			ID3D12Device* directx_device,
			_In_ const D3D12_RESOURCE_DESC* pDesc,
			D3D12_RESOURCE_STATES InitialResourceState,
			_In_opt_ const D3D12_CLEAR_VALUE* pOptimizedClearValue,
			REFIID riidResource,
			_COM_Outptr_opt_ void** ppvResource
			)> deal_func
	);
};
}

#pragma once
#include "render/render_config.h"
#include "dx12_rhi.h"
#include <DirectXTex.h>
#include <wincodec.h>
//#include <LoaderHelpers.h>

namespace luna::render
{

class DX12SwapChain;

struct DxResourceCopyLayout
{
	std::vector<D3D12_PLACED_SUBRESOURCE_FOOTPRINT> pLayouts;
	std::vector<UINT> pNumRows;
	std::vector<UINT64> pRowSizeInBytes;
	UINT64 pTotalBytes = 0;
};


class RENDER_API DX12Resource : public RHIResource
{
public:
	DX12Resource(const RHIBufferDesc& buffer_desc);

	DX12Resource(const SamplerDesc& desc);

	DX12Resource(const RHITextureDesc& textureDesc, const RHIResDesc& resDesc);

	DX12Resource(uint32_t backBufferId, DX12SwapChain* swapchain);

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

	void SetLastState(const D3D12_RESOURCE_STATES& newDesc) { mLastState = newDesc; };

	Microsoft::WRL::ComPtr<ID3D12Resource> mDxRes;
	void* mMapPointer = nullptr;
	D3D12_RESOURCE_DESC mDxDesc = {};
	D3D12_SAMPLER_DESC mDxSamplerDesc = {};
	DxResourceCopyLayout mLayout;
	D3D12_RESOURCE_STATES mLastState;
};
}

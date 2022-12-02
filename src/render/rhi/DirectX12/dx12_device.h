#pragma once
#include "render/pch.h"
#include "dx12_rhi.h"
#include "render/rhi/rhi_frame_buffer.h"
#include "render/rhi/DirectX12/dx12_device.h"
#include "render/rhi/DirectX12/dx12_descriptor_impl.h"

namespace luna::render
{
class LDirectx12DynamicRingBuffer;



class RENDER_API DX12FrameBuffer : public RHIFrameBuffer
{
public:
	DX12FrameBuffer(const FrameBufferDesc& desc) :
		mRTV(desc.mColor[0]),
		mDsv(desc.mDepthStencil)
	{

	};

	RHIViewPtr mRTV;
	RHIViewPtr mDsv;
};

class RENDER_API DX12Device : public RHIDevice
{
	Microsoft::WRL::ComPtr<LDirectXGIFactory> m_dxgi_factory;
	Microsoft::WRL::ComPtr<ID3D12Device> m_device;
	D3D12_FEATURE_DATA_D3D12_OPTIONS m_feature_desc;
	LDirectx12DynamicRingBuffer* m_upload_buffer = nullptr;
	std::unordered_map<D3D12_DESCRIPTOR_HEAP_TYPE, TRHIPtr<Dx12GpuDescriptorHeap>> gpu_descriptor_heap;
	std::unordered_map<D3D12_DESCRIPTOR_HEAP_TYPE, TRHIPtr<Dx12CpuDescriptorHeap>> cpu_descriptor_heap;
public:
	DX12Device();
	~DX12Device() override;
	Dx12CpuDescriptorHeap* GetCpuDescriptorHeapByType(const D3D12_DESCRIPTOR_HEAP_TYPE type) { return cpu_descriptor_heap[type].get(); };
	Dx12GpuDescriptorHeap* GetGpuDescriptorHeapByType(const D3D12_DESCRIPTOR_HEAP_TYPE type) { return gpu_descriptor_heap[type].get(); };;
	void InitDescriptorHeap();


	ID3D12Device* GetDx12Device() const { return m_device.Get(); }

	LDirectXGIFactory* GetDxGIFactory() const {return m_dxgi_factory.Get();}

	RHIResourcePtr CreateSamplerExt(const SamplerDesc& desc) override;

	RHIViewPtr CreateView(const ViewDesc&) override;

	RHIBindingSetPtr CreateBindingSet(RHIDescriptorPool* pool, RHIBindingSetLayoutPtr layout) override;

	RHIResourcePtr CreateTextureExt(const RHITextureDesc& textureDesc, const RHIResDesc& resDesc) override;

	RHIResourcePtr CreateBufferExt(const RHIBufferDesc& buffer_desc) override;

	D3D12_FEATURE_DATA_D3D12_OPTIONS GetDeviceFeatureDesc()
	{
		return m_feature_desc;
	}

	bool CheckIfResourceHeapTire2() override;

	RHIBindingSetLayoutPtr CreateBindingSetLayout(const std::vector<RHIBindPoint>& bindKeys) override;
	
	TRHIPtr<RHIMemory> AllocMemory(const RHIMemoryDesc& desc, uint32_t memoryBits = 0) override;

	void CopyInitDataToResource(void* initData, size_t dataSize, RHIResourcePtr sourceDataLayout, RHIResourcePtr resDynamic) override;

	RHIViewPtr CreateDescriptor(
		const RHIViewType& descriptor_type,
		size_t descriptor_offset,
		size_t descriptor_size,
		RHIDescriptorPool* descriptor_heap
	) override;
	RHIFencePtr CreateFence() override;
	RHIDescriptorPoolPtr CreateDescriptorPool(DescriptorPoolDesc desc) override;

	RHIGraphicCmdListPtr CreateCommondList(RHICmdListType pipeline_type) override;

	RHIResourcePtr CreateUniforBuffer(size_t uniform_buffer_size) override;

	RHIResourcePtr CreateTextureArray(
		RHITextureDesc& texture_desc,
		RHIResDesc& texture_resource_desc,
		LVector<const byte*> init_datas,		
		TextureMemoryType init_memory_type
	) override;

	RHIResourcePtr CreateCommitTexture(
		RHITextureDesc& texture_desc,
		RHIResDesc& texture_resource_desc,
		byte* init_data,
		size_t data_size,
		TextureMemoryType init_memory_type
	) override;

	RHIResourcePtr CreateCommitBuffer(const RHIBufferDesc& buffer_desc, void* init_data) override;

	RHIResourcePtr CreatePlacedBuffer(
		RHIMemory* res_heap,
		size_t offset,
		size_t buffer_size,
		LResState res_state
	) override;
	RHIResourcePtr CreatePlacedTexture(
		RHIMemory* res_heap,
		size_t offset,
		const RHITextureDesc& texture_desc,
		const RHIResDesc& texture_resource_desc
	) override;
	RHIResourcePtr CreateEmptyTexture(
		const RHITextureDesc& texture_desc,
		const RHIResDesc& texture_resource_desc
	) override;
	size_t CountResourceSizeByDesc(const RHIResDesc& res_desc) override;

	RHIFrameBufferPtr CreateFrameBuffer(const FrameBufferDesc& desc) override;

	RHIRenderPassPtr CreateRenderPass(const RenderPassDesc& desc) override;


private:
	bool InitDeviceData() override;
	void GetHardwareAdapter(IDXGIFactory2* pFactory, IDXGIAdapter1** ppAdapter);
	//全局反射信息注册
	void AddDirectXEnumVariable();
	RHIShaderBlobPtr CreateShader(const RHIShaderDesc& desc) override;
	RHIPipelineStatePtr CreatePipeline(const RHIPipelineStateDesc& desc) override;
	inline void MemcopyDataToResource(
		_In_ const D3D12_MEMCPY_DEST* pDest,
		_In_ const D3D12_SUBRESOURCE_DATA* pSrc,
		SIZE_T RowSizeInBytes,
		UINT NumRows,
		UINT NumSlices)
	{
		for (UINT z = 0; z < NumSlices; ++z)
		{
			BYTE* pDestSlice = reinterpret_cast<BYTE*>(pDest->pData) + pDest->SlicePitch * z;
			const BYTE* pSrcSlice = reinterpret_cast<const BYTE*>(pSrc->pData) + pSrc->SlicePitch * z;
			for (UINT y = 0; y < NumRows; ++y)
			{
				memcpy(pDestSlice + pDest->RowPitch * y,
					pSrcSlice + pSrc->RowPitch * y,
					RowSizeInBytes);
			}
		}
	}
};
}

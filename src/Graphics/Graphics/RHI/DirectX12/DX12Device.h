#pragma once
#include "Graphics/RenderConfig.h"
#include "DX12Pch.h"
#include "Graphics/RHI/RHIFrameBuffer.h"
#include "Graphics/RHI/DirectX12/DX12Device.h"
#include "Graphics/RHI/DirectX12/DX12DescriptorImpl.h"
#include "D3D12MemAlloc.h"
namespace luna::graphics
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

class RENDER_API DX12MemoryManagerPool
{
	D3D12MA::Allocator* mAllocator;
public:
	DX12MemoryManagerPool();
	~DX12MemoryManagerPool();
	bool Create(const D3D12MA::ALLOCATOR_DESC& allocatorInfo);
	bool BindResourceMemory(RHIHeapType type, const D3D12_RESOURCE_DESC& memoryRequire, Microsoft::WRL::ComPtr<ID3D12Resource>& resource, D3D12MA::Allocation* &allocation);
	bool FreeResourceMemory(D3D12MA::Allocation*& allocation);
};

class RENDER_API DX12Device : public RHIDevice
{
	Microsoft::WRL::ComPtr<LDirectXGIFactory> m_dxgi_factory;
	Microsoft::WRL::ComPtr<ID3D12Device> m_device;
	Microsoft::WRL::ComPtr<IDXGIAdapter1> mHardwareAdapter;
	D3D12_FEATURE_DATA_D3D12_OPTIONS m_feature_desc;
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

	RHIResourcePtr CreateTextureExt(const RHIResDesc& resDesc) override;

	RHIResourcePtr CreateBufferExt(const RHIBufferDesc& buffer_desc) override;

	D3D12_FEATURE_DATA_D3D12_OPTIONS GetDeviceFeatureDesc()
	{
		return m_feature_desc;
	}

	bool CheckIfResourceHeapTire2() override;

	RHIBindingSetLayoutPtr CreateBindingSetLayout(const std::vector<RHIBindPoint>& bindKeys, const std::unordered_map<ShaderParamID, RHIPushConstantValue>& mBindConstKeys) override;
	
	TRHIPtr<RHIMemory> AllocMemory(const RHIMemoryDesc& desc, uint32_t memoryBits = 0) override;

	RHIViewPtr CreateDescriptor(
		const RHIViewType& descriptor_type,
		size_t descriptor_offset,
		size_t descriptor_size,
		RHIDescriptorPool* descriptor_heap
	) override;

	RHIFencePtr CreateFence() override;

	RHIDescriptorPoolPtr CreateDescriptorPool(const DescriptorPoolDesc& desc) override;

	RHISinglePoolSingleCmdListPtr CreateSinglePoolSingleCommondList(RHICmdListType type) override;
	
	RHISinglePoolMultiCmdListPtr CreateSinglePoolMultiCommondList(RHICmdListType type) override;
	
	RHIMultiFrameCmdListPtr CreateMultiFrameCommondList(size_t frameCount, RHICmdListType type) override;

	size_t CountResourceSizeByDesc(const RHIResDesc& res_desc) override;

	RHIFrameBufferPtr CreateFrameBuffer(const FrameBufferDesc& desc) override;

	RHIRenderPassPtr CreateRenderPass(const RenderPassDesc& desc) override;

	DX12MemoryManagerPool& GetDirectXDmaPool() { return mDefaultDmaMemoryAllocator; }

private:
	DX12MemoryManagerPool mDefaultDmaMemoryAllocator;

	bool InitDeviceData() override;

	void GetHardwareAdapter(IDXGIFactory2* pFactory, IDXGIAdapter1** ppAdapter);
	//全局反射信息注册
	void AddDirectXEnumVariable();

	RHIShaderBlobPtr CreateShader(const RHIShaderDesc& desc) override;
	
	RHIPipelineStatePtr CreatePipelineGraphic(
		const RHIPipelineStateGraphDrawDesc& desc,
		const RHIVertexLayout& inputLayout,
		const RenderPassDesc& renderPassDesc
	) override;

	RHIPipelineStatePtr CreatePipelineCompute(const RHIPipelineStateComputeDesc& desc) override;

	RHICmdSignaturePtr CreateCmdSignature(RHIPipelineState* pipeline, const LArray<CommandArgDesc>& allCommondDesc) override;

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

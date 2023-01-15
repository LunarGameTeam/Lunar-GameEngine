#pragma once

#include "render/render_config.h"
#include "render/rhi/DirectX12/dx12_rhi.h"


using namespace Microsoft::WRL;

namespace luna::render
{

struct Dx12DescriptorList
{
	D3D12_DESCRIPTOR_HEAP_TYPE mHeapType;
	D3D12_CPU_DESCRIPTOR_HANDLE mCPUHandle;
	D3D12_GPU_DESCRIPTOR_HANDLE mGPUHandle;
	size_t mPerDescriptorSize;
	size_t mDescriptorDescriptorNum;
};
struct Dx12DescriptorSet
{
	LUnorderedMap<D3D12_DESCRIPTOR_HEAP_TYPE, Dx12DescriptorList> mDescriptorLists;
};

class Dx12GpuDescriptorHeap;
class RENDER_API DX12GpuDescriptorSegment : public RHIObject
{
	D3D12_DESCRIPTOR_HEAP_TYPE mType;
	Dx12GpuDescriptorHeap* mHeap;
	D3D12_CPU_DESCRIPTOR_HANDLE mCPUHandle;
	D3D12_GPU_DESCRIPTOR_HANDLE mGPUHandle;
	size_t mDescriptorNumber;
	size_t mSegmentOffset;
	const uint32_t mPerDescriptorSize;
public:
	DX12GpuDescriptorSegment(D3D12_DESCRIPTOR_HEAP_TYPE type,Dx12GpuDescriptorHeap* heap,const size_t offset, uint32_t per_descriptor_size,const uint32_t size);
	void AllocDescriptorSet(const uint32_t size,Dx12DescriptorSet& m_set);
	void Reset() { mSegmentOffset = 0; };
};

class DX12TypedDescriptorHeap : public RHIObject
{
public:
	DX12TypedDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE type,const size_t max_size);

	void Init();

	const D3D12_CPU_DESCRIPTOR_HANDLE& GetCpuHandle() { return m_cpu_handle; };

	D3D12_CPU_DESCRIPTOR_HANDLE GetCpuHandle(const size_t offset)
	{
		D3D12_CPU_DESCRIPTOR_HANDLE offset_handle = m_cpu_handle;
		offset_handle.ptr += offset * per_descriptor_size;
		return offset_handle;
	};

	ID3D12DescriptorHeap* const GetDeviceHeap() { return mDXHeap.Get(); };
protected:
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mDXHeap;
	D3D12_DESCRIPTOR_HEAP_TYPE m_type;
	D3D12_CPU_DESCRIPTOR_HANDLE m_cpu_handle = D3D12_CPU_DESCRIPTOR_HANDLE(0);
	size_t m_size = 0;
	uint32_t per_descriptor_size = 0;
private:
	virtual void InitImpl(
		ID3D12Device* device,
		D3D12_DESCRIPTOR_HEAP_DESC& heap_desc,
		const size_t max_size,
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>& heap
	) {};
	
};

class Dx12CpuDescriptorHeap : public DX12TypedDescriptorHeap
{
	LQueue<size_t> m_empty_descriptor_id;
	LUnorderedSet<size_t> m_used_descriptor_id;
public:
	Dx12CpuDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE type, const size_t max_size):DX12TypedDescriptorHeap(type, max_size)
	{
		for (uint32_t i = 0; i < max_size; ++i)
		{
			m_empty_descriptor_id.push(i);
		}
	};
	size_t AllocView();
	void ReleaseView(const size_t view_id);
private:
	void InitImpl(
		ID3D12Device* device,
		D3D12_DESCRIPTOR_HEAP_DESC& heap_desc,
		const size_t max_size,
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>& heap
	) override;
};

class Dx12GpuDescriptorHeap : public DX12TypedDescriptorHeap
{
	D3D12_GPU_DESCRIPTOR_HANDLE m_gpu_handle = D3D12_GPU_DESCRIPTOR_HANDLE(0);
	size_t m_offset = 0;
public:
	Dx12GpuDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE type, const size_t max_size) :DX12TypedDescriptorHeap(type, max_size) {};
	TRHIPtr<DX12GpuDescriptorSegment> AllocPool(size_t count);
	void Reset()
	{
		m_offset = 0;
	}
	const D3D12_GPU_DESCRIPTOR_HANDLE& GetGpuHandle() { return m_gpu_handle; };
private:
	void Resize()
	{
		assert(0);
	};
	void InitImpl(
		ID3D12Device* device,
		D3D12_DESCRIPTOR_HEAP_DESC& heap_desc,
		const size_t max_size,
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>& heap
	) override;

};


}

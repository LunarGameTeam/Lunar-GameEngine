#include "DX12DescriptorImpl.h"
#include "DX12DescriptorPool.h"
#include "DX12Resource.h"

#include "Graphics/RenderModule.h"
#include "Graphics/RHI//DirectX12/DX12Device.h"

using namespace luna;


namespace luna::render
{

DX12GpuDescriptorSegment::DX12GpuDescriptorSegment(
	D3D12_DESCRIPTOR_HEAP_TYPE type,
	Dx12GpuDescriptorHeap* heap ,
	const size_t offset,
	const uint32_t per_descriptor_size,
	const uint32_t size
):
	mType(type),
	mHeap(heap),
	mDescriptorNumber(size),
	mSegmentOffset(0),
	mPerDescriptorSize(per_descriptor_size)
{
	mCPUHandle.ptr = heap->GetCpuHandle().ptr + offset * per_descriptor_size;
	mGPUHandle.ptr = heap->GetGpuHandle().ptr + offset * per_descriptor_size;
}

void DX12GpuDescriptorSegment::AllocDescriptorSet(const uint32_t size, Dx12DescriptorSet& m_set)
{
	LUNA_ASSERT(mSegmentOffset + size < mDescriptorNumber);
	m_set.mDescriptorLists[mType].mDescriptorDescriptorNum = size;
	m_set.mDescriptorLists[mType].mCPUHandle.ptr = mCPUHandle.ptr + mSegmentOffset * mPerDescriptorSize;
	m_set.mDescriptorLists[mType].mGPUHandle.ptr = mGPUHandle.ptr + mSegmentOffset * mPerDescriptorSize;
	m_set.mDescriptorLists[mType].mPerDescriptorSize = mPerDescriptorSize;
	m_set.mDescriptorLists[mType].mHeapType = mType;
	mSegmentOffset += size;
}

DX12TypedDescriptorHeap::DX12TypedDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE type, const size_t max_size) :
	m_type(type),
	m_size(max_size)
{

}

void DX12TypedDescriptorHeap::Init()
{
	ID3D12Device* device = sRenderModule->GetDevice<DX12Device>()->GetDx12Device();

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> heap;
	D3D12_DESCRIPTOR_HEAP_DESC heap_desc = {};
	heap_desc.NumDescriptors = static_cast<uint32_t>(m_size);
	heap_desc.Type = m_type;
	InitImpl(device, heap_desc, m_size, mDXHeap);
	m_cpu_handle = mDXHeap->GetCPUDescriptorHandleForHeapStart();
	per_descriptor_size = device->GetDescriptorHandleIncrementSize(m_type);
}

void Dx12CpuDescriptorHeap::InitImpl(
	ID3D12Device* device,
	D3D12_DESCRIPTOR_HEAP_DESC& heap_desc,
	const size_t max_size,
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>& heap
)
{
	heap_desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	bool if_succeed = SUCCEEDED(device->CreateDescriptorHeap(&heap_desc, IID_PPV_ARGS(&heap)));
	if (!if_succeed)
	{
		LogError("Device", "failed create descriptor heap ,check log in compiler");
		assert(0);
	}
}

size_t Dx12CpuDescriptorHeap::AllocView()
{
	if (m_empty_descriptor_id.empty())
	{
		LogError("Device", "failed alloc descriptor ,heap is full ");
		assert(0);
	}
	size_t used_id = m_empty_descriptor_id.front();
	m_empty_descriptor_id.pop();
	m_used_descriptor_id.insert(used_id);
	return used_id;
}

void Dx12CpuDescriptorHeap::ReleaseView(const size_t view_id)
{
	if (m_used_descriptor_id.find(view_id) == m_used_descriptor_id.end())
	{
		LogError("Device", "failed free descriptor ,index not find ");
		assert(0);
	}
	m_used_descriptor_id.erase(view_id);
	m_empty_descriptor_id.push(view_id);
}

TRHIPtr<DX12GpuDescriptorSegment> Dx12GpuDescriptorHeap::AllocPool(size_t count)
{
	if (m_offset + count > m_size)
	{
		//Ŀǰ�в�֧���Զ��Ĵ�С��������������setpool̫����Ҫ��device��ʼ����ʱ�򣬵���һ�����GPU�������ѵ�size
		Resize();
	}
	TRHIPtr<DX12GpuDescriptorSegment> m_value =  CreateRHIObject<DX12GpuDescriptorSegment>(m_type,this, m_offset,per_descriptor_size,count);
	m_offset += count;
	return m_value;
}

void Dx12GpuDescriptorHeap::InitImpl(
	ID3D12Device* device,
	D3D12_DESCRIPTOR_HEAP_DESC& heap_desc,
	const size_t max_size,
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>& heap
)
{
	heap_desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	bool if_succeed = SUCCEEDED(device->CreateDescriptorHeap(&heap_desc, IID_PPV_ARGS(&heap)));
	if (!if_succeed)
	{
		LogError("Device", "failed create descriptor heap ,check log in compiler");
		assert(0);
	}
	m_gpu_handle = mDXHeap->GetGPUDescriptorHandleForHeapStart();
}


//TRHIPtr<DX12Descriptor> DX12TypedDescriptorHeap::Allocate(size_t count)
//{
//	ID3D12Device* device = sRenderModule->GetDevice<DX12Device>()->GetDx12Device();
//	if (m_offset + count > m_size)
//		Resize(std::max(m_offset + count, 2 * (m_size + 1)));
//	m_offset += count;
//	TRHIPtr<DX12Descriptor> descriptor = CreateRHIObject< DX12Descriptor>();
//	descriptor->mCPUHandle.ptr = m_cpu_handle.ptr;
//	descriptor->mCPUHandle.ptr += m_offset * device->GetDescriptorHandleIncrementSize(m_type);
//	descriptor->mGPUHandle.ptr = m_gpu_handle.ptr;
//	descriptor->mGPUHandle.ptr += m_offset * device->GetDescriptorHandleIncrementSize(m_type);
//	return descriptor;
//}
//
//void DX12TypedDescriptorHeap::Resize(size_t req_size)
//{
//	ID3D12Device* device = sRenderModule->GetDevice<DX12Device>()->GetDx12Device();
//	if (m_size >= req_size)
//		return;
//
//	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> heap;
//	D3D12_DESCRIPTOR_HEAP_DESC heap_desc = {};
//	heap_desc.NumDescriptors = static_cast<uint32_t>(req_size);
//	heap_desc.Type = m_type;
//	if (m_type == D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV || m_type == D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER)
//		heap_desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
//	else
//		heap_desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
//
//	SUCCEEDED(device->CreateDescriptorHeap(&heap_desc, IID_PPV_ARGS(&heap)));
//	if (m_size > 0)
//	{
//		device->CopyDescriptorsSimple(
//			static_cast<uint32_t>(m_size),
//			heap->GetCPUDescriptorHandleForHeapStart(),
//			mDXHeap->GetCPUDescriptorHandleForHeapStart(),
//			m_type);
//	}
//
//	m_size = heap_desc.NumDescriptors;
//	mDXHeap = heap;
//	m_cpu_handle = mDXHeap->GetCPUDescriptorHandleForHeapStart();
//	if (m_type == D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV || m_type == D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER)
//	{
//		m_gpu_handle = mDXHeap->GetGPUDescriptorHandleForHeapStart();
//	}
//	
//}

}

#include "dx12_command_list.h"
#include "dx12_descriptor_impl.h"
#include "dx12_descriptor_pool.h"
#include "dx12_resource.h"
#include "dx12_device.h"
#include "pix3.h"
#include "dx12_binding_set.h"
#include "dx12_view.h"

#include "render/render_module.h"


namespace luna::render
{

DX12GraphicCmdList::DX12GraphicCmdList(RHICmdListType cmdListType)
	:RHIGraphicCmdList(cmdListType)
{

	//获取directx设备
	ID3D12Device* dxDevice = sRenderModule->GetDevice<DX12Device>()->GetDx12Device();

	//创建并解锁alloctor
	D3D12_COMMAND_LIST_TYPE dxCmdListType;
	GetDirectXCommondlistType(cmdListType, dxCmdListType);
	//创建commondlist
	HRESULT hr;
	hr = dxDevice->CreateCommandAllocator(dxCmdListType, IID_PPV_ARGS(&mDxCmdAllocator));
	assert(SUCCEEDED(hr));
	hr = dxDevice->CreateCommandList(0, dxCmdListType, mDxCmdAllocator.Get(), nullptr, IID_PPV_ARGS(&mDxCmdList));
	assert(SUCCEEDED(hr));
}
void DX12GraphicCmdList::DrawIndexedInstanced(
	uint32_t IndexCountPerInstance,
	uint32_t InstanceCount,
	uint32_t StartIndexLocation,
	int32_t BaseVertexLocation,
	int32_t StartInstanceLocation)
{
	mDxCmdList->DrawIndexedInstanced(IndexCountPerInstance, InstanceCount, StartIndexLocation,
		BaseVertexLocation, StartInstanceLocation);
};

void DX12GraphicCmdList::SetVertexBuffer(const LVector<RHIVertexBufferDesc>& bufferView, int32_t slot)
{
	std::vector<D3D12_VERTEX_BUFFER_VIEW> dx12BufferView(bufferView.size());	
	for (int32_t idx = 0; idx < bufferView.size(); ++idx)
	{
		const RHIVertexBufferDesc& view = bufferView[idx];
		DX12ResourceNew* dx12Res = bufferView[idx].mVertexRes->As<DX12ResourceNew>();
		dx12BufferView[idx].BufferLocation = dx12Res->mDxRes->GetGPUVirtualAddress();
		dx12BufferView[idx].SizeInBytes = dx12Res->mResSize;
		dx12BufferView[idx].StrideInBytes = view.mVertexLayout->GetSize();
	}
	mDxCmdList->IASetVertexBuffers(slot, bufferView.size(), dx12BufferView.data());
};

void DX12GraphicCmdList::SetIndexBuffer(RHIResource* indexRes)
{
	DX12ResourceNew* dx12Res = indexRes->As<DX12ResourceNew>();
	D3D12_INDEX_BUFFER_VIEW ibv;
	ibv.BufferLocation = dx12Res->mDxRes->GetGPUVirtualAddress();
	ibv.Format = DXGI_FORMAT_R32_UINT;
	ibv.SizeInBytes = (UINT)dx12Res->mResSize;
	mDxCmdList->IASetIndexBuffer(&ibv);
}

void DX12GraphicCmdList::CopyBufferToBuffer(
	RHIResource* target_resource,
	uint32_t target_copy_offset,
	RHIResource* source_resource,
	uint32_t source_copy_offset,
	const size_t copy_size
)
{
	DX12ResourceNew* dst_dx_res = reinterpret_cast<DX12ResourceNew*>(target_resource);
	DX12ResourceNew* src_dx_res = reinterpret_cast<DX12ResourceNew*>(source_resource);
	mDxCmdList->CopyBufferRegion(
		dst_dx_res->mDxRes.Get(),
		target_copy_offset,
		src_dx_res->mDxRes.Get(),
		source_copy_offset,
		copy_size
	);
}

void DX12GraphicCmdList::CopyBufferToTexture(
	RHIResource* target_resource,
	uint32_t target_subresource,
	RHIResource* source_resource,
	uint32_t source_subresource
)
{
	DX12ResourceNew* dx12DstRes = target_resource->As<DX12ResourceNew>();
	DX12ResourceNew* dx12SrcRes = source_resource->As<DX12ResourceNew>();

	std::vector<D3D12_PLACED_SUBRESOURCE_FOOTPRINT> footprints;
	footprints.resize(dx12DstRes->mDxDesc.DepthOrArraySize);

// 	for (int i = 0; i < dx12DstRes->mDxDesc.DepthOrArraySize; ++i)
// 	{
// 		footprints[i].Footprint.RowPitch = 
// 	}
// 	.pNumRows.resize(mDxDesc.DepthOrArraySize);
// 	dxMemoryLayout->mDxRequire.pRowSizeInBytes.resize(mDxDesc.DepthOrArraySize);
// 	device->GetCopyableFootprints(
// 		&mDxDesc,
// 		0,
// 		mDxDesc.DepthOrArraySize,
// 		0,
// 		dxMemoryLayout->mDxRequire.pLayouts.data(),
// 		dxMemoryLayout->mDxRequire.pNumRows.data(),
// 		dxMemoryLayout->mDxRequire.pRowSizeInBytes.data(),
// 		&dxMemoryLayout->mDxRequire.pTotalBytes
// 	);
	
	for (UINT i = 0; i < footprints.size(); ++i)
	{
		D3D12_TEXTURE_COPY_LOCATION dstCopy = {};
		dstCopy.Type = D3D12_TEXTURE_COPY_TYPE::D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
		dstCopy.pResource = dx12DstRes->mDxRes.Get();
		dstCopy.SubresourceIndex = i;
		D3D12_TEXTURE_COPY_LOCATION dxSrcCopy;
		dxSrcCopy.Type = D3D12_TEXTURE_COPY_TYPE::D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
		dxSrcCopy.pResource = dx12SrcRes->mDxRes.Get();
		dxSrcCopy.PlacedFootprint = footprints[i];
		mDxCmdList->CopyTextureRegion(&dstCopy, 0, 0, 0, &dxSrcCopy, nullptr);
	}

}

void DX12GraphicCmdList::CopyTextureRegionByFootprint(
	RHIResource* dstRes,
	const size_t dsrSubResNum,
	RHIResource* srcRes,
	const RHIPlacedSubResFootprint& srcFootprint
)
{
	DX12Resource* dst_dx_res = dstRes->As<DX12Resource>();
	DX12Resource* src_dx_res = srcRes->As<DX12Resource>();
	D3D12_TEXTURE_COPY_LOCATION target_copy_loc;
	target_copy_loc.Type = D3D12_TEXTURE_COPY_TYPE::D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
	target_copy_loc.pResource = dst_dx_res->GetResource();
	target_copy_loc.SubresourceIndex = dsrSubResNum;
	D3D12_TEXTURE_COPY_LOCATION source_copy_loc;
	auto foot_print = GetFootPrint(srcFootprint);
	source_copy_loc.Type = D3D12_TEXTURE_COPY_TYPE::D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
	source_copy_loc.pResource = src_dx_res->GetResource();
	source_copy_loc.PlacedFootprint = foot_print;
	mDxCmdList->CopyTextureRegion(&target_copy_loc, 0, 0, 0, &source_copy_loc, nullptr);
}

void DX12GraphicCmdList::SetPipelineState(
	RHIPipelineState* pipeline
)
{
	ID3D12PipelineState* dx_pipeline = nullptr;
	if (pipeline != nullptr)
	{
		//获取pipeline
		DX12PipelineState* pipeline_data = dynamic_cast<DX12PipelineState*>(pipeline);
		dx_pipeline = pipeline_data->GetPipeLine();
		mDxCmdList->SetPipelineState(dx_pipeline);		
		mDxCmdList->SetGraphicsRootSignature(pipeline_data->mBindingSetLayout->As<DX12BindingSetLayout>()->GetRootSignature());
	}
}

void DX12GraphicCmdList::SetDrawPrimitiveTopology(const RHIPrimitiveTopology& primitive_topology)
{
	mDxCmdList->IASetPrimitiveTopology(GetDirectXPrimitiveTopology(primitive_topology));
};

void DX12GraphicCmdList::SetBiningSetLayoutExt(
	RHIBindingSetLayout* root_signature
)
{
	DX12BindingSetLayout* my_root_signature = dynamic_cast<DX12BindingSetLayout*>(root_signature);
	auto d3d_root_signature = my_root_signature->GetRootSignature();
	if (mCmdListType == RHICmdListType::Graphic3D)
	{
		mDxCmdList->SetGraphicsRootSignature(d3d_root_signature);
	}
	else if (mCmdListType == RHICmdListType::Compute)
	{
		mDxCmdList->SetComputeRootSignature(d3d_root_signature);
	}
}

void DX12GraphicCmdList::BindDescriptorHeap()
{
	//获取directx设备
	DX12Device* dxDevice = sRenderModule->GetDevice<DX12Device>();
	Dx12GpuDescriptorHeap* gpu_heap_srv = dxDevice->GetGpuDescriptorHeapByType(D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	Dx12GpuDescriptorHeap* gpu_heap_sampler = dxDevice->GetGpuDescriptorHeapByType(D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);
	ID3D12DescriptorHeap * const heap_value[2] = { gpu_heap_srv->GetDeviceHeap(),gpu_heap_sampler->GetDeviceHeap()};
	mDxCmdList->SetDescriptorHeaps(2, heap_value);
}

//void DX12GraphicCmdList::BindDescriptorToPipeline(
//	RHIView* descriptor_data,
//	const int32_t bind_offset,
//	const int32_t slot)
//{
//	DX12View* dx12_descriptor_object = dynamic_cast<DX12View*>(descriptor_data);
//	D3D12_GPU_DESCRIPTOR_HANDLE dx_bind_handle = dx12_descriptor_object->GetCpuHandle();
//	mDxCmdList->SetGraphicsRootDescriptorTable(slot, dx_bind_handle);
//}


void DX12GraphicCmdList::ResourceBarrier(
	RHIResource* target_resource,
	const LResState& resource_state_target
)
{
	return;
};

void DX12GraphicCmdList::SetViewPort(
	size_t x,
	size_t y,
	size_t width,
	size_t height
)
{
	D3D12_VIEWPORT new_vp;
	new_vp.TopLeftX = x;
	new_vp.TopLeftY = y;
	new_vp.Width = (float)width;
	new_vp.Height = (float)height;
	new_vp.MaxDepth = 1.0f;
	new_vp.MinDepth = 0.0f;
	mDxCmdList->RSSetViewports(1, &new_vp);
}

void DX12GraphicCmdList::SetScissorRects(
	size_t x,
	size_t y,
	size_t width,
	size_t height
)
{
	D3D12_RECT render_rect;
	render_rect.left = (LONG)x;
	render_rect.top = (LONG)y;
	render_rect.right = (LONG)width;
	render_rect.bottom = (LONG)height;
	mDxCmdList->RSSetScissorRects(1, &render_rect);
}

void DX12GraphicCmdList::ClearRTView(
	RHIView* descriptor_rtv,
	LVector4f clear_color, 
	int x /*= 0*/,
	int y /*= 0*/,
	int width /*= 0*/,
	int height /*= 0*/
)
{
	DX12View* dx12View = descriptor_rtv->As<DX12View>();
	D3D12_RECT render_rect;
	if (width == 0)
		width = dx12View->GetRessource()->mWidth;
	if (height == 0)
		height = dx12View->GetRessource()->mHeight;
	render_rect.left = (LONG)x;
	render_rect.top = (LONG)y;
	render_rect.right = (LONG)width;
	render_rect.bottom = (LONG)height;
	mDxCmdList->ClearRenderTargetView(dx12View->GetCpuHandle(), clear_color.data(),
		1, &render_rect);
}

void DX12GraphicCmdList::ClearDSView(
	uint32_t sr_left,
	uint32_t sr_top,
	uint32_t sr_right,
	uint32_t sr_bottom,
	RHIView* descriptor_dsv,
	float depth,
	uint8_t stencil
)
{
	D3D12_RECT render_rect;
	render_rect.left = (LONG)sr_left;
	render_rect.top = (LONG)sr_top;
	render_rect.right = (LONG)sr_right;
	render_rect.bottom = (LONG)sr_bottom;
	DX12View* descriptor_dx12 = dynamic_cast<DX12View*>(descriptor_dsv);
	mDxCmdList->ClearDepthStencilView
	(
		descriptor_dx12->GetCpuHandle(),
		D3D12_CLEAR_FLAGS::D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAGS::D3D12_CLEAR_FLAG_STENCIL,
		depth,
		stencil,
		1,
		&render_rect
	);
}

void DX12GraphicCmdList::BeginEvent(const LString& event_str)
{
	PIXBeginEvent(mDxCmdList.Get(), 0, StringToWstring(event_str.c_str()).c_str());
}

void DX12GraphicCmdList::EndEvent()
{
	PIXEndEvent(mDxCmdList.Get());
}

D3D12_PRIMITIVE_TOPOLOGY DX12GraphicCmdList::GetDirectXPrimitiveTopology(
	const RHIPrimitiveTopology& primitive_topology)
{
	//PRIMITIVE_TOPOLOGY_(.*)\n
	//case RHIPrimitiveTopology::PRIMITIVE_TOPOLOGY_$1: \nreturn D3D_PRIMITIVE_TOPOLOGY_$1
	switch (primitive_topology)
	{
	case RHIPrimitiveTopology::PRIMITIVE_TOPOLOGY_UNDEFINED:
		return D3D_PRIMITIVE_TOPOLOGY_UNDEFINED;
	case RHIPrimitiveTopology::PRIMITIVE_TOPOLOGY_POINTLIST:
		return D3D_PRIMITIVE_TOPOLOGY_POINTLIST;
	case RHIPrimitiveTopology::PRIMITIVE_TOPOLOGY_LINELIST:
		return D3D_PRIMITIVE_TOPOLOGY_LINELIST;
	case RHIPrimitiveTopology::PRIMITIVE_TOPOLOGY_LINESTRIP:
		return D3D_PRIMITIVE_TOPOLOGY_LINESTRIP;
	case RHIPrimitiveTopology::PRIMITIVE_TOPOLOGY_TRIANGLELIST:
		return D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	case RHIPrimitiveTopology::PRIMITIVE_TOPOLOGY_TRIANGLESTRIP:
		return D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
	case RHIPrimitiveTopology::PRIMITIVE_TOPOLOGY_LINELIST_ADJ:
		return D3D_PRIMITIVE_TOPOLOGY_LINELIST_ADJ;
	case RHIPrimitiveTopology::PRIMITIVE_TOPOLOGY_LINESTRIP_ADJ:
		return D3D_PRIMITIVE_TOPOLOGY_LINESTRIP_ADJ;
	case RHIPrimitiveTopology::PRIMITIVE_TOPOLOGY_TRIANGLELIST_ADJ:
		return D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST_ADJ;
	case RHIPrimitiveTopology::PRIMITIVE_TOPOLOGY_TRIANGLESTRIP_ADJ:
		return D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP_ADJ;
	case RHIPrimitiveTopology::PRIMITIVE_TOPOLOGY_1_CONTROL_POINT_PATCHLIST:
		return D3D_PRIMITIVE_TOPOLOGY_1_CONTROL_POINT_PATCHLIST;
	case RHIPrimitiveTopology::PRIMITIVE_TOPOLOGY_2_CONTROL_POINT_PATCHLIST:
		return D3D_PRIMITIVE_TOPOLOGY_2_CONTROL_POINT_PATCHLIST;
	case RHIPrimitiveTopology::PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST:
		return D3D_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST;
	case RHIPrimitiveTopology::PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST:
		return D3D_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST;
	case RHIPrimitiveTopology::PRIMITIVE_TOPOLOGY_5_CONTROL_POINT_PATCHLIST:
		return D3D_PRIMITIVE_TOPOLOGY_5_CONTROL_POINT_PATCHLIST;
	case RHIPrimitiveTopology::PRIMITIVE_TOPOLOGY_6_CONTROL_POINT_PATCHLIST:
		return D3D_PRIMITIVE_TOPOLOGY_6_CONTROL_POINT_PATCHLIST;
	case RHIPrimitiveTopology::PRIMITIVE_TOPOLOGY_7_CONTROL_POINT_PATCHLIST:
		return D3D_PRIMITIVE_TOPOLOGY_7_CONTROL_POINT_PATCHLIST;
	case RHIPrimitiveTopology::PRIMITIVE_TOPOLOGY_8_CONTROL_POINT_PATCHLIST:
		return D3D_PRIMITIVE_TOPOLOGY_8_CONTROL_POINT_PATCHLIST;
	case RHIPrimitiveTopology::PRIMITIVE_TOPOLOGY_9_CONTROL_POINT_PATCHLIST:
		return D3D_PRIMITIVE_TOPOLOGY_9_CONTROL_POINT_PATCHLIST;
	case RHIPrimitiveTopology::PRIMITIVE_TOPOLOGY_10_CONTROL_POINT_PATCHLIST:
		return D3D_PRIMITIVE_TOPOLOGY_10_CONTROL_POINT_PATCHLIST;
	case RHIPrimitiveTopology::PRIMITIVE_TOPOLOGY_11_CONTROL_POINT_PATCHLIST:
		return D3D_PRIMITIVE_TOPOLOGY_11_CONTROL_POINT_PATCHLIST;
	case RHIPrimitiveTopology::PRIMITIVE_TOPOLOGY_12_CONTROL_POINT_PATCHLIST:
		return D3D_PRIMITIVE_TOPOLOGY_12_CONTROL_POINT_PATCHLIST;
	case RHIPrimitiveTopology::PRIMITIVE_TOPOLOGY_13_CONTROL_POINT_PATCHLIST:
		return D3D_PRIMITIVE_TOPOLOGY_13_CONTROL_POINT_PATCHLIST;
	case RHIPrimitiveTopology::PRIMITIVE_TOPOLOGY_14_CONTROL_POINT_PATCHLIST:
		return D3D_PRIMITIVE_TOPOLOGY_14_CONTROL_POINT_PATCHLIST;
	case RHIPrimitiveTopology::PRIMITIVE_TOPOLOGY_15_CONTROL_POINT_PATCHLIST:
		return D3D_PRIMITIVE_TOPOLOGY_15_CONTROL_POINT_PATCHLIST;
	case RHIPrimitiveTopology::PRIMITIVE_TOPOLOGY_16_CONTROL_POINT_PATCHLIST:
		return D3D_PRIMITIVE_TOPOLOGY_16_CONTROL_POINT_PATCHLIST;
	case RHIPrimitiveTopology::PRIMITIVE_TOPOLOGY_17_CONTROL_POINT_PATCHLIST:
		return D3D_PRIMITIVE_TOPOLOGY_17_CONTROL_POINT_PATCHLIST;
	case RHIPrimitiveTopology::PRIMITIVE_TOPOLOGY_18_CONTROL_POINT_PATCHLIST:
		return D3D_PRIMITIVE_TOPOLOGY_18_CONTROL_POINT_PATCHLIST;
	case RHIPrimitiveTopology::PRIMITIVE_TOPOLOGY_19_CONTROL_POINT_PATCHLIST:
		return D3D_PRIMITIVE_TOPOLOGY_19_CONTROL_POINT_PATCHLIST;
	case RHIPrimitiveTopology::PRIMITIVE_TOPOLOGY_20_CONTROL_POINT_PATCHLIST:
		return D3D_PRIMITIVE_TOPOLOGY_20_CONTROL_POINT_PATCHLIST;
	case RHIPrimitiveTopology::PRIMITIVE_TOPOLOGY_21_CONTROL_POINT_PATCHLIST:
		return D3D_PRIMITIVE_TOPOLOGY_21_CONTROL_POINT_PATCHLIST;
	case RHIPrimitiveTopology::PRIMITIVE_TOPOLOGY_22_CONTROL_POINT_PATCHLIST:
		return D3D_PRIMITIVE_TOPOLOGY_22_CONTROL_POINT_PATCHLIST;
	case RHIPrimitiveTopology::PRIMITIVE_TOPOLOGY_23_CONTROL_POINT_PATCHLIST:
		return D3D_PRIMITIVE_TOPOLOGY_23_CONTROL_POINT_PATCHLIST;
	case RHIPrimitiveTopology::PRIMITIVE_TOPOLOGY_24_CONTROL_POINT_PATCHLIST:
		return D3D_PRIMITIVE_TOPOLOGY_24_CONTROL_POINT_PATCHLIST;
	case RHIPrimitiveTopology::PRIMITIVE_TOPOLOGY_25_CONTROL_POINT_PATCHLIST:
		return D3D_PRIMITIVE_TOPOLOGY_25_CONTROL_POINT_PATCHLIST;
	case RHIPrimitiveTopology::PRIMITIVE_TOPOLOGY_26_CONTROL_POINT_PATCHLIST:
		return D3D_PRIMITIVE_TOPOLOGY_26_CONTROL_POINT_PATCHLIST;
	case RHIPrimitiveTopology::PRIMITIVE_TOPOLOGY_27_CONTROL_POINT_PATCHLIST:
		return D3D_PRIMITIVE_TOPOLOGY_27_CONTROL_POINT_PATCHLIST;
	case RHIPrimitiveTopology::PRIMITIVE_TOPOLOGY_28_CONTROL_POINT_PATCHLIST:
		return D3D_PRIMITIVE_TOPOLOGY_28_CONTROL_POINT_PATCHLIST;
	case RHIPrimitiveTopology::PRIMITIVE_TOPOLOGY_29_CONTROL_POINT_PATCHLIST:
		return D3D_PRIMITIVE_TOPOLOGY_29_CONTROL_POINT_PATCHLIST;
	case RHIPrimitiveTopology::PRIMITIVE_TOPOLOGY_30_CONTROL_POINT_PATCHLIST:
		return D3D_PRIMITIVE_TOPOLOGY_30_CONTROL_POINT_PATCHLIST;
	case RHIPrimitiveTopology::PRIMITIVE_TOPOLOGY_31_CONTROL_POINT_PATCHLIST:
		return D3D_PRIMITIVE_TOPOLOGY_31_CONTROL_POINT_PATCHLIST;
	case RHIPrimitiveTopology::PRIMITIVE_TOPOLOGY_32_CONTROL_POINT_PATCHLIST:
		return D3D_PRIMITIVE_TOPOLOGY_32_CONTROL_POINT_PATCHLIST;
	default:
		return D3D_PRIMITIVE_TOPOLOGY_UNDEFINED;
	}
	return D3D_PRIMITIVE_TOPOLOGY_UNDEFINED;
}

void DX12GraphicCmdList::BeginRenderPass(RHIRenderPass* pass, RHIFrameBuffer* buffer)
{
	DX12FrameBuffer* dx12Target = static_cast<DX12FrameBuffer*>(buffer);
	RHIView* descriptor_rtv = dx12Target->mRTV;
	const int32_t bind_offset_rtv = 0;
	RHIView* descriptor_dsv = dx12Target->mDsv;
	const int32_t bind_offset_dsv = 0;

	DX12View* rtv_directx_pointer = static_cast<DX12View*>(descriptor_rtv);
	DX12View* dsv_directx_pointer = static_cast<DX12View*>(descriptor_dsv);
	D3D12_CPU_DESCRIPTOR_HANDLE* rtv_final = nullptr;
	D3D12_CPU_DESCRIPTOR_HANDLE* dsv_final = nullptr;
	D3D12_CPU_DESCRIPTOR_HANDLE cpu_handle_rtv, cpu_handle_dsv;
	if (rtv_directx_pointer != nullptr)
	{
		cpu_handle_rtv = rtv_directx_pointer->GetCpuHandle();
		rtv_final = &cpu_handle_rtv;
	}
	if (dsv_directx_pointer != nullptr)
	{
		cpu_handle_dsv = dsv_directx_pointer->GetCpuHandle();
		dsv_final = &cpu_handle_dsv;
	}
	mDxCmdList->OMSetRenderTargets(1, rtv_final, false, dsv_final);
}

void DX12GraphicCmdList::EndRenderPass()
{

}

void DX12GraphicCmdList::Reset()
{
	if (mClosed)
	{
		assert(SUCCEEDED(mDxCmdAllocator->Reset()));
		assert(SUCCEEDED(mDxCmdList->Reset(mDxCmdAllocator.Get(), nullptr)));
	}
	mClosed = false;
}

void DX12GraphicCmdList::CloseCommondList()
{
	if (!mClosed)
	{
		assert(SUCCEEDED(mDxCmdList->Close()));
		mClosed = true;
	}
}

void DX12GraphicCmdList::BindDesriptorSetExt(RHIBindingSetPtr bindingSet)
{
	DX12BindingSet* dx12BindingSet = bindingSet->As<DX12BindingSet>();
	for (auto& it : dx12BindingSet->m_layout->GetLayout())
	{
		Dx12DescriptorSet& descriptor_set = dx12BindingSet->mDescriptorSets[it.first];
		if(it.second.each_range_input.size() > 0)
		{
			D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle;
			gpuHandle.ptr = descriptor_set.mDescriptorLists[D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV].mGPUHandle.ptr;
			mDxCmdList->SetGraphicsRootDescriptorTable(it.second.root_table_indedx_input, gpuHandle);
		}
		if (it.second.each_range_sampler.size() > 0)
		{
			D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle;
			gpuHandle.ptr = descriptor_set.mDescriptorLists[D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER].mGPUHandle.ptr;
			mDxCmdList->SetGraphicsRootDescriptorTable(it.second.root_table_indedx_sampler, gpuHandle);
		}
	}
}

void DX12GraphicCmdList::ResourceBarrierExt(const ResourceBarrierDesc& barrier)
{
	std::vector<D3D12_RESOURCE_BARRIER> dxBarriers;
	if (!barrier.mBarrierRes)
	{
		assert(false);
		return;
	}
	if (barrier.mStateBefore == ResourceState::kRaytracingAccelerationStructure)
		return;

	DX12ResourceNew* dxRes = barrier.mBarrierRes->As<DX12ResourceNew>();
	D3D12_RESOURCE_STATES dx_state_before = DxConvertState(barrier.mStateBefore);
	D3D12_RESOURCE_STATES dx_state_after = DxConvertState(barrier.mStateAfter);
	if (dx_state_before == dx_state_after)
		return;

	assert(barrier.mBaseMipLevel + barrier.mMipLevels <= dxRes->mDxDesc.MipLevels);
	assert(barrier.mBaseDepth + barrier.mDepth <= dxRes->mDxDesc.DepthOrArraySize);

	if (barrier.mBaseMipLevel == 0 && barrier.mMipLevels == dxRes->mDxDesc.MipLevels &&
		barrier.mBaseDepth == 0 && barrier.mDepth == dxRes->mDxDesc.DepthOrArraySize)
	{
		dxBarriers.emplace_back(CD3DX12_RESOURCE_BARRIER::Transition(dxRes->mDxRes.Get(), dx_state_before, dx_state_after));
	}
	else
	{
		for (uint32_t i = barrier.mBaseMipLevel; i < barrier.mBaseMipLevel + barrier.mMipLevels; ++i)
		{
			for (uint32_t j = barrier.mBaseDepth; j < barrier.mBaseDepth + barrier.mDepth; ++j)
			{
				uint32_t subresource = i + j * dxRes->mMipLevels;
				dxBarriers.emplace_back(CD3DX12_RESOURCE_BARRIER::Transition(dxRes->mDxRes.Get(), dx_state_before, dx_state_after, subresource));
			}
		}
	}
	mDxCmdList->ResourceBarrier((UINT)dxBarriers.size(), dxBarriers.data());
}

}
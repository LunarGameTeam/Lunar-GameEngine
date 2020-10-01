#include"PancyBufferDx12.h"

using namespace LunarEngine;
//基础缓冲区
PancyBasicBuffer::PancyBasicBuffer(const bool& if_could_reload) :PancyCommonVirtualResource<PancyCommonBufferDesc>(if_could_reload)
{
}
LResult PancyBasicBuffer::WriteDataToBuffer(void* cpu_data_pointer, const LunarResourceSize& data_size)
{
	auto check_error = CopyCpuDataToBufferGpu(cpu_data_pointer, data_size);
	if (!check_error.m_IsOK)
	{
		return check_error;
	}
	return g_Succeed;
}
LResult PancyBasicBuffer::CopyCpuDataToBufferGpu(void* cpu_data_pointer, const LunarResourceSize& data_size)
{
	if (data_size > subresources_size)
	{
		LResult error_message;
		LunarDebugLogError(E_FAIL, "could not copy data to GPU, size too large than buffer", error_message);

		return error_message;
	}
	//获取用于拷贝的commond list
	PancyRenderCommandList* copy_render_list;
	PancyThreadIdGPU copy_render_list_ID;
	auto check_error = ThreadPoolGPUControl::GetInstance()->GetResourceLoadContex()->GetThreadPool(D3D12_COMMAND_LIST_TYPE_COPY)->GetEmptyRenderlist(NULL, &copy_render_list, copy_render_list_ID);
	if (!check_error.m_IsOK)
	{
		return check_error;
	}
	//拷贝资源数据
	check_error = PancyDynamicRingBuffer::GetInstance()->CopyDataToGpu(copy_render_list, cpu_data_pointer, data_size, *buffer_data);
	if (!check_error.m_IsOK)
	{
		return check_error;
	}
	copy_render_list->UnlockPrepare();
	//提交渲染命令
	ThreadPoolGPUControl::GetInstance()->GetResourceLoadContex()->GetThreadPool(D3D12_COMMAND_LIST_TYPE_COPY)->SubmitRenderlist(1, &copy_render_list_ID);
	//分配等待眼位
	PancyFenceIdGPU WaitFence;
	ThreadPoolGPUControl::GetInstance()->GetResourceLoadContex()->GetThreadPool(D3D12_COMMAND_LIST_TYPE_COPY)->SetGpuBrokenFence(WaitFence);
	check_error = buffer_data->SetResourceCopyBrokenFence(WaitFence);
	if (!check_error.m_IsOK)
	{
		return check_error;
	}
	return g_Succeed;
}
LResult PancyBasicBuffer::LoadResoureDataByDesc(const PancyCommonBufferDesc& resource_desc)
{
	LResult check_error;
	ComPtr<ID3D12Resource> resource_data;
	//在d3d层级上创建一个单独堆的buffer资源
	D3D12_HEAP_TYPE heap_type = D3D12_HEAP_TYPE::D3D12_HEAP_TYPE_DEFAULT;
	D3D12_RESOURCE_STATES resource_build_state = D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_COMMON;
	switch (resource_desc.buffer_type)
	{
	case Buffer_ShaderResource_static:
	{
		heap_type = D3D12_HEAP_TYPE::D3D12_HEAP_TYPE_DEFAULT;
		break;
	}
	case Buffer_ShaderResource_dynamic:
	{
		heap_type = D3D12_HEAP_TYPE::D3D12_HEAP_TYPE_UPLOAD;
		resource_build_state = D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_GENERIC_READ;
		break;
	}
	case Buffer_Constant:
	{
		heap_type = D3D12_HEAP_TYPE::D3D12_HEAP_TYPE_UPLOAD;
		resource_build_state = D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_GENERIC_READ;
		break;
	}
	case Buffer_Vertex:
	{
		heap_type = D3D12_HEAP_TYPE::D3D12_HEAP_TYPE_DEFAULT;
		break;
	}
	case Buffer_Index:
	{
		heap_type = D3D12_HEAP_TYPE::D3D12_HEAP_TYPE_DEFAULT;
		break;
	}
	case Buffer_UnorderedAccess_static:
	{
		heap_type = D3D12_HEAP_TYPE::D3D12_HEAP_TYPE_DEFAULT;
		break;
	}
	default:
		break;
	}
	HRESULT hr = PancyDx12DeviceBasic::GetInstance()->GetD3dDevice()->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(heap_type),
		D3D12_HEAP_FLAG_NONE,
		&resource_desc.buffer_res_desc,
		resource_build_state,
		nullptr,
		IID_PPV_ARGS(&resource_data)
	);
	if (FAILED(hr))
	{
		LResult error_message;
		LunarDebugLogError(E_FAIL, "Create commit resource error", error_message);

		return error_message;
	}
	//计算缓冲区的大小，创建资源块
	PancyDx12DeviceBasic::GetInstance()->GetD3dDevice()->GetCopyableFootprints(&resource_desc.buffer_res_desc, 0, 1, 0, nullptr, nullptr, nullptr, &subresources_size);
	if (resource_desc.buffer_res_desc.Width != subresources_size)
	{
		LResult error_message;
		LunarDebugLogError(0, "buffer resource size dismatch, maybe it's not a buffer resource", error_message);

		return error_message;
	}
	buffer_data = new ResourceBlockGpu(subresources_size, resource_data, heap_type, resource_build_state);
	if (heap_type == D3D12_HEAP_TYPE::D3D12_HEAP_TYPE_UPLOAD)
	{
		check_error = buffer_data->GetCpuMapPointer(&map_pointer);
		if (!check_error.m_IsOK)
		{
			return check_error;
		}
	}

	//如果需要拷贝数据，将数据拷贝到buffer中
	if (resource_desc.buffer_type == Buffer_Index || resource_desc.buffer_type == Buffer_Vertex || resource_desc.buffer_type == Buffer_ShaderResource_static)
	{
		char* buffer_memory = NULL;
		//todo:从文件中读取buffer
		if (resource_desc.buffer_data_file != "")
		{
			/*
			check_error = CopyCpuDataToBufferGpu(cpu_data_pointer, data_size);
			if (!check_error.if_succeed)
			{
				return check_error;
			}
			*/
		}

	}
	return g_Succeed;
}
bool PancyBasicBuffer::CheckIfResourceLoadFinish()
{
	PancyResourceLoadState now_load_state = buffer_data->GetResourceLoadingState();
	if (now_load_state == PancyResourceLoadState::RESOURCE_LOAD_CPU_FINISH || now_load_state == PancyResourceLoadState::RESOURCE_LOAD_GPU_FINISH)
	{
		return true;
	}
	return false;
}
PancyBasicBuffer::~PancyBasicBuffer()
{
	if (buffer_data != NULL)
	{
		delete buffer_data;
	}
}
ResourceBlockGpu* LunarEngine::GetBufferResourceData(VirtualResourcePointer& virtual_pointer, LResult& check_error)
{
	check_error = g_Succeed;
	auto now_buffer_resource_value = virtual_pointer.GetResourceData();
	if (now_buffer_resource_value->GetResourceTypeName() != typeid(PancyBasicBuffer).name())
	{
		LResult error_message;
		LunarDebugLogError(E_FAIL, "the vertex resource is not a buffer", error_message);

		check_error = error_message;
	}
	const PancyBasicBuffer* buffer_real_pointer = dynamic_cast<const PancyBasicBuffer*>(now_buffer_resource_value);
	auto gpu_buffer_data = buffer_real_pointer->GetGpuResourceData();
	if (gpu_buffer_data != NULL)
	{
		return gpu_buffer_data;
	}
	return NULL;
}
LResult LunarEngine::BuildBufferResource(
	const std::string& name_resource_in,
	PancyCommonBufferDesc& resource_data,
	VirtualResourcePointer& id_need,
	bool if_allow_repeat
)
{
	auto check_error = PancyGlobelResourceControl::GetInstance()->LoadResource<PancyBasicBuffer>(
		name_resource_in,
		&resource_data,
		typeid(PancyCommonBufferDesc*).name(),
		sizeof(PancyCommonBufferDesc),
		id_need,
		if_allow_repeat
		);
	if (!check_error.m_IsOK)
	{
		return check_error;
	}
	return g_Succeed;
}
LResult LunarEngine::BuildBufferResourceFromMemory(
	const std::string& name_resource_in,
	VirtualResourcePointer& id_need,
	void* data_pointer,
	const LunarResourceSize& resource_size,
	bool if_allow_repeat,
	D3D12_RESOURCE_FLAGS flag
)
{
	//创建存储动画数据的缓冲区资源(静态缓冲区)
	PancyCommonBufferDesc buffer_desc;
	buffer_desc.buffer_type = Buffer_ShaderResource_static;
	buffer_desc.buffer_res_desc.Alignment = 0;
	buffer_desc.buffer_res_desc.DepthOrArraySize = 1;
	buffer_desc.buffer_res_desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	buffer_desc.buffer_res_desc.Flags = flag;
	buffer_desc.buffer_res_desc.Height = 1;
	buffer_desc.buffer_res_desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	buffer_desc.buffer_res_desc.MipLevels = 1;
	buffer_desc.buffer_res_desc.SampleDesc.Count = 1;
	buffer_desc.buffer_res_desc.SampleDesc.Quality = 0;
	buffer_desc.buffer_res_desc.Width = SizeAligned(resource_size, 65536);
	LResult check_error = BuildBufferResource(
		name_resource_in,
		buffer_desc,
		id_need,
		true
	);
	if (!check_error.m_IsOK)
	{
		return check_error;
	}
	//获取数据拷贝commondlist
	PancyRenderCommandList* copy_render_list;
	PancyThreadIdGPU copy_render_list_ID;
	check_error = ThreadPoolGPUControl::GetInstance()->GetResourceLoadContex()->GetThreadPool(D3D12_COMMAND_LIST_TYPE_COPY)->GetEmptyRenderlist(NULL, &copy_render_list, copy_render_list_ID);
	if (!check_error.m_IsOK)
	{
		return check_error;
	}
	//拷贝资源数据
	ResourceBlockGpu* buffer_gpu_resource = GetBufferResourceData(id_need, check_error);
	if (!check_error.m_IsOK)
	{
		return check_error;
	}
	check_error = PancyDynamicRingBuffer::GetInstance()->CopyDataToGpu(copy_render_list, data_pointer, resource_size, *buffer_gpu_resource);
	if (!check_error.m_IsOK)
	{
		return check_error;
	}
	copy_render_list->UnlockPrepare();
	//提交渲染命令
	ThreadPoolGPUControl::GetInstance()->GetResourceLoadContex()->GetThreadPool(D3D12_COMMAND_LIST_TYPE_COPY)->SubmitRenderlist(1, &copy_render_list_ID);
	//分配等待眼位
	PancyFenceIdGPU WaitFence;
	ThreadPoolGPUControl::GetInstance()->GetResourceLoadContex()->GetThreadPool(D3D12_COMMAND_LIST_TYPE_COPY)->SetGpuBrokenFence(WaitFence);
	check_error = buffer_gpu_resource->SetResourceCopyBrokenFence(WaitFence);
	if (!check_error.m_IsOK)
	{
		return check_error;
	}
	return g_Succeed;
}
LResult LunarEngine::LoadBufferResourceFromFile(
	const std::string& name_resource_in,
	VirtualResourcePointer& id_need
)
{
	auto check_error = PancyGlobelResourceControl::GetInstance()->LoadResource<PancyBasicBuffer>(
		name_resource_in,
		id_need,
		false
		);
	if (!check_error.m_IsOK)
	{
		return check_error;
	}
	return g_Succeed;
}
void LunarEngine::InitBufferJsonReflect()
{
	InitNewEnumValue(Buffer_ShaderResource_static);
	InitNewEnumValue(Buffer_ShaderResource_dynamic);
	InitNewEnumValue(Buffer_Constant);
	InitNewEnumValue(Buffer_Vertex);
	InitNewEnumValue(Buffer_Index);
	InitNewEnumValue(Buffer_UnorderedAccess_static);

	InitNewEnumValue(D3D12_RESOURCE_DIMENSION_UNKNOWN);
	InitNewEnumValue(D3D12_RESOURCE_DIMENSION_BUFFER);
	InitNewEnumValue(D3D12_RESOURCE_DIMENSION_TEXTURE1D);
	InitNewEnumValue(D3D12_RESOURCE_DIMENSION_TEXTURE2D);
	InitNewEnumValue(D3D12_RESOURCE_DIMENSION_TEXTURE3D);

	InitNewEnumValue(DXGI_FORMAT_UNKNOWN);
	InitNewEnumValue(DXGI_FORMAT_R32G32B32A32_TYPELESS);
	InitNewEnumValue(DXGI_FORMAT_R32G32B32A32_FLOAT);
	InitNewEnumValue(DXGI_FORMAT_R32G32B32A32_UINT);
	InitNewEnumValue(DXGI_FORMAT_R32G32B32A32_SINT);
	InitNewEnumValue(DXGI_FORMAT_R32G32B32_TYPELESS);
	InitNewEnumValue(DXGI_FORMAT_R32G32B32_FLOAT);
	InitNewEnumValue(DXGI_FORMAT_R32G32B32_UINT);
	InitNewEnumValue(DXGI_FORMAT_R32G32B32_SINT);
	InitNewEnumValue(DXGI_FORMAT_R16G16B16A16_TYPELESS);
	InitNewEnumValue(DXGI_FORMAT_R16G16B16A16_FLOAT);
	InitNewEnumValue(DXGI_FORMAT_R16G16B16A16_UNORM);
	InitNewEnumValue(DXGI_FORMAT_R16G16B16A16_UINT);
	InitNewEnumValue(DXGI_FORMAT_R16G16B16A16_SNORM);
	InitNewEnumValue(DXGI_FORMAT_R16G16B16A16_SINT);
	InitNewEnumValue(DXGI_FORMAT_R32G32_TYPELESS);
	InitNewEnumValue(DXGI_FORMAT_R32G32_FLOAT);
	InitNewEnumValue(DXGI_FORMAT_R32G32_UINT);
	InitNewEnumValue(DXGI_FORMAT_R32G32_SINT);
	InitNewEnumValue(DXGI_FORMAT_R32G8X24_TYPELESS);
	InitNewEnumValue(DXGI_FORMAT_D32_FLOAT_S8X24_UINT);
	InitNewEnumValue(DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS);
	InitNewEnumValue(DXGI_FORMAT_X32_TYPELESS_G8X24_UINT);
	InitNewEnumValue(DXGI_FORMAT_R10G10B10A2_TYPELESS);
	InitNewEnumValue(DXGI_FORMAT_R10G10B10A2_UNORM);
	InitNewEnumValue(DXGI_FORMAT_R10G10B10A2_UINT);
	InitNewEnumValue(DXGI_FORMAT_R11G11B10_FLOAT);
	InitNewEnumValue(DXGI_FORMAT_R8G8B8A8_TYPELESS);
	InitNewEnumValue(DXGI_FORMAT_R8G8B8A8_UNORM);
	InitNewEnumValue(DXGI_FORMAT_R8G8B8A8_UNORM_SRGB);
	InitNewEnumValue(DXGI_FORMAT_R8G8B8A8_UINT);
	InitNewEnumValue(DXGI_FORMAT_R8G8B8A8_SNORM);
	InitNewEnumValue(DXGI_FORMAT_R8G8B8A8_SINT);
	InitNewEnumValue(DXGI_FORMAT_R16G16_TYPELESS);
	InitNewEnumValue(DXGI_FORMAT_R16G16_FLOAT);
	InitNewEnumValue(DXGI_FORMAT_R16G16_UNORM);
	InitNewEnumValue(DXGI_FORMAT_R16G16_UINT);
	InitNewEnumValue(DXGI_FORMAT_R16G16_SNORM);
	InitNewEnumValue(DXGI_FORMAT_R16G16_SINT);
	InitNewEnumValue(DXGI_FORMAT_R32_TYPELESS);
	InitNewEnumValue(DXGI_FORMAT_D32_FLOAT);
	InitNewEnumValue(DXGI_FORMAT_R32_FLOAT);
	InitNewEnumValue(DXGI_FORMAT_R32_UINT);
	InitNewEnumValue(DXGI_FORMAT_R32_SINT);
	InitNewEnumValue(DXGI_FORMAT_R24G8_TYPELESS);
	InitNewEnumValue(DXGI_FORMAT_D24_UNORM_S8_UINT);
	InitNewEnumValue(DXGI_FORMAT_R24_UNORM_X8_TYPELESS);
	InitNewEnumValue(DXGI_FORMAT_X24_TYPELESS_G8_UINT);
	InitNewEnumValue(DXGI_FORMAT_R8G8_TYPELESS);
	InitNewEnumValue(DXGI_FORMAT_R8G8_UNORM);
	InitNewEnumValue(DXGI_FORMAT_R8G8_UINT);
	InitNewEnumValue(DXGI_FORMAT_R8G8_SNORM);
	InitNewEnumValue(DXGI_FORMAT_R8G8_SINT);
	InitNewEnumValue(DXGI_FORMAT_R16_TYPELESS);
	InitNewEnumValue(DXGI_FORMAT_R16_FLOAT);
	InitNewEnumValue(DXGI_FORMAT_D16_UNORM);
	InitNewEnumValue(DXGI_FORMAT_R16_UNORM);
	InitNewEnumValue(DXGI_FORMAT_R16_UINT);
	InitNewEnumValue(DXGI_FORMAT_R16_SNORM);
	InitNewEnumValue(DXGI_FORMAT_R16_SINT);
	InitNewEnumValue(DXGI_FORMAT_R8_TYPELESS);
	InitNewEnumValue(DXGI_FORMAT_R8_UNORM);
	InitNewEnumValue(DXGI_FORMAT_R8_UINT);
	InitNewEnumValue(DXGI_FORMAT_R8_SNORM);
	InitNewEnumValue(DXGI_FORMAT_R8_SINT);
	InitNewEnumValue(DXGI_FORMAT_A8_UNORM);
	InitNewEnumValue(DXGI_FORMAT_R1_UNORM);
	InitNewEnumValue(DXGI_FORMAT_R9G9B9E5_SHAREDEXP);
	InitNewEnumValue(DXGI_FORMAT_R8G8_B8G8_UNORM);
	InitNewEnumValue(DXGI_FORMAT_G8R8_G8B8_UNORM);
	InitNewEnumValue(DXGI_FORMAT_BC1_TYPELESS);
	InitNewEnumValue(DXGI_FORMAT_BC1_UNORM);
	InitNewEnumValue(DXGI_FORMAT_BC1_UNORM_SRGB);
	InitNewEnumValue(DXGI_FORMAT_BC2_TYPELESS);
	InitNewEnumValue(DXGI_FORMAT_BC2_UNORM);
	InitNewEnumValue(DXGI_FORMAT_BC2_UNORM_SRGB);
	InitNewEnumValue(DXGI_FORMAT_BC3_TYPELESS);
	InitNewEnumValue(DXGI_FORMAT_BC3_UNORM);
	InitNewEnumValue(DXGI_FORMAT_BC3_UNORM_SRGB);
	InitNewEnumValue(DXGI_FORMAT_BC4_TYPELESS);
	InitNewEnumValue(DXGI_FORMAT_BC4_UNORM);
	InitNewEnumValue(DXGI_FORMAT_BC4_SNORM);
	InitNewEnumValue(DXGI_FORMAT_BC5_TYPELESS);
	InitNewEnumValue(DXGI_FORMAT_BC5_UNORM);
	InitNewEnumValue(DXGI_FORMAT_BC5_SNORM);
	InitNewEnumValue(DXGI_FORMAT_B5G6R5_UNORM);
	InitNewEnumValue(DXGI_FORMAT_B5G5R5A1_UNORM);
	InitNewEnumValue(DXGI_FORMAT_B8G8R8A8_UNORM);
	InitNewEnumValue(DXGI_FORMAT_B8G8R8X8_UNORM);
	InitNewEnumValue(DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM);
	InitNewEnumValue(DXGI_FORMAT_B8G8R8A8_TYPELESS);
	InitNewEnumValue(DXGI_FORMAT_B8G8R8A8_UNORM_SRGB);
	InitNewEnumValue(DXGI_FORMAT_B8G8R8X8_TYPELESS);
	InitNewEnumValue(DXGI_FORMAT_B8G8R8X8_UNORM_SRGB);
	InitNewEnumValue(DXGI_FORMAT_BC6H_TYPELESS);
	InitNewEnumValue(DXGI_FORMAT_BC6H_UF16);
	InitNewEnumValue(DXGI_FORMAT_BC6H_SF16);
	InitNewEnumValue(DXGI_FORMAT_BC7_TYPELESS);
	InitNewEnumValue(DXGI_FORMAT_BC7_UNORM);
	InitNewEnumValue(DXGI_FORMAT_BC7_UNORM_SRGB);
	InitNewEnumValue(DXGI_FORMAT_AYUV);
	InitNewEnumValue(DXGI_FORMAT_Y410);
	InitNewEnumValue(DXGI_FORMAT_Y416);
	InitNewEnumValue(DXGI_FORMAT_NV12);
	InitNewEnumValue(DXGI_FORMAT_P010);
	InitNewEnumValue(DXGI_FORMAT_P016);
	InitNewEnumValue(DXGI_FORMAT_420_OPAQUE);
	InitNewEnumValue(DXGI_FORMAT_YUY2);
	InitNewEnumValue(DXGI_FORMAT_Y210);
	InitNewEnumValue(DXGI_FORMAT_Y216);
	InitNewEnumValue(DXGI_FORMAT_NV11);
	InitNewEnumValue(DXGI_FORMAT_AI44);
	InitNewEnumValue(DXGI_FORMAT_IA44);
	InitNewEnumValue(DXGI_FORMAT_P8);
	InitNewEnumValue(DXGI_FORMAT_A8P8);
	InitNewEnumValue(DXGI_FORMAT_B4G4R4A4_UNORM);
	InitNewEnumValue(DXGI_FORMAT_P208);
	InitNewEnumValue(DXGI_FORMAT_V208);
	InitNewEnumValue(DXGI_FORMAT_V408);
	InitNewEnumValue(DXGI_FORMAT_FORCE_UINT);

	InitNewEnumValue(D3D12_TEXTURE_LAYOUT_UNKNOWN);
	InitNewEnumValue(D3D12_TEXTURE_LAYOUT_ROW_MAJOR);
	InitNewEnumValue(D3D12_TEXTURE_LAYOUT_64KB_UNDEFINED_SWIZZLE);
	InitNewEnumValue(D3D12_TEXTURE_LAYOUT_64KB_STANDARD_SWIZZLE);

	InitNewEnumValue(D3D12_RESOURCE_FLAG_NONE);
	InitNewEnumValue(D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET);
	InitNewEnumValue(D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);
	InitNewEnumValue(D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);
	InitNewEnumValue(D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE);
	InitNewEnumValue(D3D12_RESOURCE_FLAG_ALLOW_CROSS_ADAPTER);
	InitNewEnumValue(D3D12_RESOURCE_FLAG_ALLOW_SIMULTANEOUS_ACCESS);
	InitNewEnumValue(D3D12_RESOURCE_FLAG_VIDEO_DECODE_REFERENCE_ONLY);

	InitNewStructToReflection(PancyCommonBufferDesc);
}


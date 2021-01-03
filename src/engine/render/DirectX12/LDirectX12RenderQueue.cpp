#include "LDirectX12RenderQueue.h"
#include "LDirectX12SwapChain.h"
using Microsoft::WRL::ComPtr;
using namespace luna;
luna::ILunarDirectXGraphicRenderQueueCore::ILunarDirectXGraphicRenderQueueCore()
{
}
luna::ILunarDirectXGraphicRenderQueueCore::~ILunarDirectXGraphicRenderQueueCore()
{

}
luna::LResult luna::ILunarDirectXGraphicRenderQueueCore::InitRenderQueueData()
{
	HRESULT hr;
	//创建直接渲染队列
	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	LResult check_error;
	ID3D12Device* directx_device = reinterpret_cast<ID3D12Device*>(ILunarGraphicDeviceCore::GetInstance()->GetVirtualDevice());
	if (directx_device == nullptr)
	{
		LResult error_message;
		LunarDebugLogError(0, "Directx device is broken ", error_message);
		return error_message;
	}
	hr = directx_device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&command_queue_direct));
	if (FAILED(hr))
	{
		LResult error_message;
		LunarDebugLogError(E_FAIL, "Create Command Queue(Direct) Error When init D3D basic", error_message);
		return error_message;
	}
	command_queue_direct->SetName(luna::StringToWstring("direct queue").c_str());
	//创建拷贝渲染队列
	D3D12_COMMAND_QUEUE_DESC queueDesc_copy = {};
	queueDesc_copy.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	queueDesc_copy.Type = D3D12_COMMAND_LIST_TYPE_COPY;
	hr = directx_device->CreateCommandQueue(&queueDesc_copy, IID_PPV_ARGS(&command_queue_copy));
	if (FAILED(hr))
	{
		LResult error_message;
		LunarDebugLogError(E_FAIL, "Create Command Queue(Copy) Error When init D3D basic", error_message);
		return error_message;
	}
	command_queue_direct->SetName(luna::StringToWstring("copy queue").c_str());
	//创建计算渲染队列
	D3D12_COMMAND_QUEUE_DESC queueDesc_compute = {};
	queueDesc_compute.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	queueDesc_compute.Type = D3D12_COMMAND_LIST_TYPE_COMPUTE;
	hr = directx_device->CreateCommandQueue(&queueDesc_compute, IID_PPV_ARGS(&command_queue_compute));
	if (FAILED(hr))
	{
		LResult error_message;
		LunarDebugLogError(E_FAIL, "Create Command Queue(Compute) Error When init D3D basic", error_message);
		return error_message;
	}
	command_queue_direct->SetName(luna::StringToWstring("Compute queue").c_str());
	return g_Succeed;
}
void luna::ILunarDirectXGraphicRenderQueueCore::ExecuteCommandLists(const int32_t& commond_list_num, LVector<ILunarGraphicRenderCommondList*> commond_list_array)
{
	if (commond_list_array.size() <= 0)
	{
		return;
	}
	ID3D12CommandList* common_list_submmited[MaxCommonListSubmit];
	if (commond_list_array.size() > MaxCommonListSubmit)
	{
		LResult error_message;
		LunarDebugLogError(0, "required commond list submmit size bigger than maxsize", error_message);
	}
	LunarGraphicPipeLineType commond_list_type;
	for (int32_t commond_list_index = 0; commond_list_index < commond_list_array.size(); ++commond_list_index)
	{
		LDx12GraphicRenderCommondList* dx12_commond_list = dynamic_cast<LDx12GraphicRenderCommondList*>(commond_list_array[commond_list_index]);
		if (dx12_commond_list == nullptr)
		{
			LResult error_message;
			LunarDebugLogError(0, "commond list type is wrong", error_message);
		}
		if (commond_list_index == 0)
		{
			commond_list_type = dx12_commond_list->GetPipelineType();
		}
		common_list_submmited[commond_list_index] = dx12_commond_list->GetCommondList();
	}
	ID3D12CommandQueue* command_queue_used = GetCommandQueueByPipelineType(commond_list_type);
	if (command_queue_used != nullptr)
	{
		command_queue_used->ExecuteCommandLists((UINT)commond_list_array.size(), common_list_submmited);
	}
}
ID3D12CommandQueue* luna::ILunarDirectXGraphicRenderQueueCore::GetCommandQueueByPipelineType(const luna::LunarGraphicPipeLineType& pipeline_type)
{
	switch (pipeline_type)
	{
	case luna::PIPELINE_GRAPHIC3D:
	{
		return command_queue_direct.Get();
	}
	case luna::PIPELINE_COMPUTE:
	{
		return command_queue_compute.Get();
	}
	case luna::PIPELINE_COPY:
	{
		return command_queue_copy.Get();
	}
	default:
	{
		LResult error_message;
		LunarDebugLogError(0, "unsurpported commond list type", error_message);
		return nullptr;
	}
	}
	return nullptr;
}
luna::ILunarGraphicRenderSwapChain* luna::ILunarDirectXGraphicRenderQueueCore::CreateSwapChain(
	const void* trarget_window,
	const LWindowRenderDesc& trarget_window_desc,
	const LunarGraphicPipeLineType& pipeline_type
)
{
	ID3D12CommandQueue* queue_data = GetCommandQueueByPipelineType(pipeline_type);
	luna::ILunarGraphicRenderSwapChain* swap_chain_object = LCreateObject<LDx12GraphicSwapChain>(queue_data, trarget_window, trarget_window_desc);
	swap_chain_object->Create();
	return swap_chain_object;
}
luna::LResult ILunarGraphicRenderQueueCore::SingleCreate()
{
	luna::LResult check_error;
	_graphic_render_queue_instance = new ILunarDirectXGraphicRenderQueueCore();
	check_error = _graphic_render_queue_instance->Create();
	if (!check_error.m_IsOK)
	{
		return check_error;
	}
	return g_Succeed;
}
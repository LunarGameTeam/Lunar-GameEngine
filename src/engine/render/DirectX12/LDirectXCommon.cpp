#include "LDirectXCommon.h"
using namespace luna;
LResult luna::GetDirectXCommondlistType(const LunarGraphicPipeLineType &type_input, D3D12_COMMAND_LIST_TYPE & type_output)
{
	switch (type_input)
	{
	case LunarGraphicPipeLineType::PIPELINE_GRAPHIC3D:
	{
		type_output = D3D12_COMMAND_LIST_TYPE::D3D12_COMMAND_LIST_TYPE_DIRECT;
		break;
	}
	case LunarGraphicPipeLineType::PIPELINE_COPY:
	{
		type_output = D3D12_COMMAND_LIST_TYPE::D3D12_COMMAND_LIST_TYPE_COPY;
		break;
	}
	case LunarGraphicPipeLineType::PIPELINE_COMPUTE:
	{
		type_output = D3D12_COMMAND_LIST_TYPE::D3D12_COMMAND_LIST_TYPE_COMPUTE;
		break;
	}
	default:
	{
		LResult error_message;
		LunarDebugLogError(0, "Unsurpported common allocator type ", error_message);
		return error_message;
	}
	}
	return g_Succeed;
}
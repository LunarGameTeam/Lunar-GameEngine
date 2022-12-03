#include "dx12_rhi.h"

#include "d3dx12.h"


namespace luna::render
{


D3D12_RESOURCE_STATES DxConvertState(ResourceState state)
{
	static std::pair<ResourceState, D3D12_RESOURCE_STATES> mapping[] = {
		{ ResourceState::kCommon, D3D12_RESOURCE_STATE_COMMON },
		{ ResourceState::kVertexAndConstantBuffer, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER },
		{ ResourceState::kIndexBuffer, D3D12_RESOURCE_STATE_INDEX_BUFFER },
		{ ResourceState::kRenderTarget, D3D12_RESOURCE_STATE_RENDER_TARGET },
		{ ResourceState::kUnorderedAccess, D3D12_RESOURCE_STATE_UNORDERED_ACCESS },
		{ ResourceState::kDepthStencilWrite, D3D12_RESOURCE_STATE_DEPTH_WRITE },
		{ ResourceState::kDepthStencilRead, D3D12_RESOURCE_STATE_DEPTH_READ },
		{ ResourceState::kNonPixelShaderResource, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE },
		{ ResourceState::kShaderReadOnly, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE },
		{ ResourceState::kIndirectArgument, D3D12_RESOURCE_STATE_INDIRECT_ARGUMENT },
		{ ResourceState::kCopyDest, D3D12_RESOURCE_STATE_COPY_DEST },
		{ ResourceState::kCopySource, D3D12_RESOURCE_STATE_COPY_SOURCE },
		{ ResourceState::kRaytracingAccelerationStructure, D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE },
		{ ResourceState::kShadingRateSource, D3D12_RESOURCE_STATE_SHADING_RATE_SOURCE },
		{ ResourceState::kPresent, D3D12_RESOURCE_STATE_PRESENT },
		{ ResourceState::kGenericRead, D3D12_RESOURCE_STATE_GENERIC_READ },
		{ ResourceState::kUndefined, D3D12_RESOURCE_STATE_COMMON },
	};

	D3D12_RESOURCE_STATES res = {};
	for (const auto& m : mapping)
	{
		if (state & m.first)
		{
			res |= m.second;
			state &= ~m.first;
		}
	}
	assert(state == 0);
	return res;
}


bool RENDER_API GetDirectXCommondlistType(RHICmdListType type_input, D3D12_COMMAND_LIST_TYPE& type_output)
{
	switch (type_input)
	{
	case RHICmdListType::Graphic3D:
	{
		type_output = D3D12_COMMAND_LIST_TYPE::D3D12_COMMAND_LIST_TYPE_DIRECT;
		break;
	}
	case RHICmdListType::Copy:
	{
		type_output = D3D12_COMMAND_LIST_TYPE::D3D12_COMMAND_LIST_TYPE_COPY;
		break;
	}
	case RHICmdListType::Compute:
	{
		type_output = D3D12_COMMAND_LIST_TYPE::D3D12_COMMAND_LIST_TYPE_COMPUTE;
		break;
	}
	default:
	{
		
		LogError("Device", "Unsurpported common allocator type ");
		return false;
	}
	}
	return true;
}

D3D12_COMPARISON_FUNC RENDER_API GetComparisionFunc(RHIComparisionFunc comparision_func)
{
	switch (comparision_func)
	{
	case RHIComparisionFunc::FuncAlways:
		return D3D12_COMPARISON_FUNC::D3D12_COMPARISON_FUNC_ALWAYS;
	case RHIComparisionFunc::FuncEqual:
		return D3D12_COMPARISON_FUNC::D3D12_COMPARISON_FUNC_EQUAL;
	case RHIComparisionFunc::FuncGreater:
		return D3D12_COMPARISON_FUNC::D3D12_COMPARISON_FUNC_GREATER;
	case RHIComparisionFunc::FuncGreaterEuqal:
		return D3D12_COMPARISON_FUNC::D3D12_COMPARISON_FUNC_GREATER_EQUAL;
	case RHIComparisionFunc::FuncLess:
		return D3D12_COMPARISON_FUNC::D3D12_COMPARISON_FUNC_LESS;
	case RHIComparisionFunc::FuncLessEqual:
		return D3D12_COMPARISON_FUNC::D3D12_COMPARISON_FUNC_LESS_EQUAL;
	case RHIComparisionFunc::FuncNever:
		return D3D12_COMPARISON_FUNC::D3D12_COMPARISON_FUNC_NEVER;
	case RHIComparisionFunc::FuncNotEqual:
		return D3D12_COMPARISON_FUNC::D3D12_COMPARISON_FUNC_NOT_EQUAL;
	default:
		return D3D12_COMPARISON_FUNC::D3D12_COMPARISON_FUNC_NEVER;
	}
}

D3D12_STATIC_BORDER_COLOR RENDER_API GetBorderColor(const LunarStaticBorderColor& border_color)
{
	switch (border_color)
	{
	case LunarStaticBorderColor::STATIC_BORDER_COLOR_OPAQUE_BLACK:
		return D3D12_STATIC_BORDER_COLOR::D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK;
	case LunarStaticBorderColor::STATIC_BORDER_COLOR_OPAQUE_WHITE:
		return D3D12_STATIC_BORDER_COLOR::D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE;
	case LunarStaticBorderColor::STATIC_BORDER_COLOR_TRANSPARENT_BLACK:
		return D3D12_STATIC_BORDER_COLOR::D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
	default:
		return D3D12_STATIC_BORDER_COLOR::D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK;
	}
}

D3D12_COMPARISON_FUNC RENDER_API GetCompareFunc(const RHIComparisionFunc& compare_func)
{
	switch (compare_func)
	{
	case RHIComparisionFunc::FuncNever:
		return D3D12_COMPARISON_FUNC_NEVER;
	case RHIComparisionFunc::FuncLess:
		return D3D12_COMPARISON_FUNC_LESS;
	case RHIComparisionFunc::FuncEqual:
		return D3D12_COMPARISON_FUNC_EQUAL;
	case RHIComparisionFunc::FuncLessEqual:
		return D3D12_COMPARISON_FUNC_LESS_EQUAL;
	case RHIComparisionFunc::FuncGreater:
		return D3D12_COMPARISON_FUNC_GREATER;
	case RHIComparisionFunc::FuncNotEqual:
		return D3D12_COMPARISON_FUNC_NOT_EQUAL;
	case RHIComparisionFunc::FuncGreaterEuqal:
		return D3D12_COMPARISON_FUNC_GREATER_EQUAL;
	case RHIComparisionFunc::FuncAlways:
		return D3D12_COMPARISON_FUNC_ALWAYS;
	default:
		return D3D12_COMPARISON_FUNC_NEVER;
	}
}

DXGI_FORMAT RENDER_API GetGraphicFormat(const RHITextureFormat& graphic_format)
{
	switch (graphic_format)
	{
	case RHITextureFormat::FORMAT_UNKNOWN:
		return DXGI_FORMAT_UNKNOWN;
	case RHITextureFormat::FORMAT_R32G32B32A32_TYPELESS:
		return DXGI_FORMAT_R32G32B32A32_TYPELESS;
	case RHITextureFormat::FORMAT_R32G32B32A32_FLOAT:
		return DXGI_FORMAT_R32G32B32A32_FLOAT;
	case RHITextureFormat::FORMAT_R32G32B32A32_UINT:
		return DXGI_FORMAT_R32G32B32A32_UINT;
	case RHITextureFormat::FORMAT_R32G32B32A32_SINT:
		return DXGI_FORMAT_R32G32B32A32_SINT;
	case RHITextureFormat::FORMAT_R32G32B32_TYPELESS:
		return DXGI_FORMAT_R32G32B32_TYPELESS;
	case RHITextureFormat::FORMAT_R32G32B32_FLOAT:
		return DXGI_FORMAT_R32G32B32_FLOAT;
	case RHITextureFormat::FORMAT_R32G32B32_UINT:
		return DXGI_FORMAT_R32G32B32_UINT;
	case RHITextureFormat::FORMAT_R32G32B32_SINT:
		return DXGI_FORMAT_R32G32B32_SINT;
	case RHITextureFormat::FORMAT_R16G16B16A16_TYPELESS:
		return DXGI_FORMAT_R16G16B16A16_TYPELESS;
	case RHITextureFormat::FORMAT_R16G16B16A16_FLOAT:
		return DXGI_FORMAT_R16G16B16A16_FLOAT;
	case RHITextureFormat::FORMAT_R16G16B16A16_UNORM:
		return DXGI_FORMAT_R16G16B16A16_UNORM;
	case RHITextureFormat::FORMAT_R16G16B16A16_UINT:
		return DXGI_FORMAT_R16G16B16A16_UINT;
	case RHITextureFormat::FORMAT_R16G16B16A16_SNORM:
		return DXGI_FORMAT_R16G16B16A16_SNORM;
	case RHITextureFormat::FORMAT_R16G16B16A16_SINT:
		return DXGI_FORMAT_R16G16B16A16_SINT;
	case RHITextureFormat::FORMAT_R32G32_TYPELESS:
		return DXGI_FORMAT_R32G32_TYPELESS;
	case RHITextureFormat::FORMAT_R32G32_FLOAT:
		return DXGI_FORMAT_R32G32_FLOAT;
	case RHITextureFormat::FORMAT_R32G32_UINT:
		return DXGI_FORMAT_R32G32_UINT;
	case RHITextureFormat::FORMAT_R32G32_SINT:
		return DXGI_FORMAT_R32G32_SINT;
	case RHITextureFormat::FORMAT_R32G8X24_TYPELESS:
		return DXGI_FORMAT_R32G8X24_TYPELESS;
	case RHITextureFormat::FORMAT_D32_FLOAT_S8X24_UINT:
		return DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
	case RHITextureFormat::FORMAT_R32_FLOAT_X8X24_TYPELESS:
		return DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS;
	case RHITextureFormat::FORMAT_X32_TYPELESS_G8X24_UINT:
		return DXGI_FORMAT_X32_TYPELESS_G8X24_UINT;
	case RHITextureFormat::FORMAT_R10G10B10A2_TYPELESS:
		return DXGI_FORMAT_R10G10B10A2_TYPELESS;
	case RHITextureFormat::FORMAT_R10G10B10A2_UNORM:
		return DXGI_FORMAT_R10G10B10A2_UNORM;
	case RHITextureFormat::FORMAT_R10G10B10A2_UINT:
		return DXGI_FORMAT_R10G10B10A2_UINT;
	case RHITextureFormat::FORMAT_R11G11B10_FLOAT:
		return DXGI_FORMAT_R11G11B10_FLOAT;
	case RHITextureFormat::FORMAT_R8G8B8A8_TYPELESS:
		return DXGI_FORMAT_R8G8B8A8_TYPELESS;
	case RHITextureFormat::FORMAT_R8G8BB8A8_UNORM:
		return DXGI_FORMAT_R8G8B8A8_UNORM;
	case RHITextureFormat::FORMAT_R8G8B8A8_UNORM_SRGB:
		return DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	case RHITextureFormat::FORMAT_R8G8B8A8_UINT:
		return DXGI_FORMAT_R8G8B8A8_UINT;
	case RHITextureFormat::FORMAT_R8G8B8A8_SNORM:
		return DXGI_FORMAT_R8G8B8A8_SNORM;
	case RHITextureFormat::FORMAT_R8G8B8A8_SINT:
		return DXGI_FORMAT_R8G8B8A8_SINT;
	case RHITextureFormat::FORMAT_R16G16_TYPELESS:
		return DXGI_FORMAT_R16G16_TYPELESS;
	case RHITextureFormat::FORMAT_R16G16_FLOAT:
		return DXGI_FORMAT_R16G16_FLOAT;
	case RHITextureFormat::FORMAT_R16G16_UNORM:
		return DXGI_FORMAT_R16G16_UNORM;
	case RHITextureFormat::FORMAT_R16G16_UINT:
		return DXGI_FORMAT_R16G16_UINT;
	case RHITextureFormat::FORMAT_R16G16_SNORM:
		return DXGI_FORMAT_R16G16_SNORM;
	case RHITextureFormat::FORMAT_R16G16_SINT:
		return DXGI_FORMAT_R16G16_SINT;
	case RHITextureFormat::FORMAT_R32_TYPELESS:
		return DXGI_FORMAT_R32_TYPELESS;
	case RHITextureFormat::FORMAT_D32_FLOAT:
		return DXGI_FORMAT_D32_FLOAT;
	case RHITextureFormat::FORMAT_R32_FLOAT:
		return DXGI_FORMAT_R32_FLOAT;
	case RHITextureFormat::FORMAT_R32_UINT:
		return DXGI_FORMAT_R32_UINT;
	case RHITextureFormat::FORMAT_R32_SINT:
		return DXGI_FORMAT_R32_SINT;
	case RHITextureFormat::FORMAT_R24G8_TYPELESS:
		return DXGI_FORMAT_R24G8_TYPELESS;
	case RHITextureFormat::FORMAT_D24_UNORM_S8_UINT:
		return DXGI_FORMAT_D24_UNORM_S8_UINT;
	case RHITextureFormat::FORMAT_R24_UNORM_X8_TYPELESS:
		return DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	case RHITextureFormat::FORMAT_X24_TYPELESS_G8_UINT:
		return DXGI_FORMAT_X24_TYPELESS_G8_UINT;
	case RHITextureFormat::FORMAT_R8G8_TYPELESS:
		return DXGI_FORMAT_R8G8_TYPELESS;
	case RHITextureFormat::FORMAT_R8G8_UNORM:
		return DXGI_FORMAT_R8G8_UNORM;
	case RHITextureFormat::FORMAT_R8G8_UINT:
		return DXGI_FORMAT_R8G8_UINT;
	case RHITextureFormat::FORMAT_R8G8_SNORM:
		return DXGI_FORMAT_R8G8_SNORM;
	case RHITextureFormat::FORMAT_R8G8_SINT:
		return DXGI_FORMAT_R8G8_SINT;
	case RHITextureFormat::FORMAT_R16_TYPELESS:
		return DXGI_FORMAT_R16_TYPELESS;
	case RHITextureFormat::FORMAT_R16_FLOAT:
		return DXGI_FORMAT_R16_FLOAT;
	case RHITextureFormat::FORMAT_D16_UNORM:
		return DXGI_FORMAT_D16_UNORM;
	case RHITextureFormat::FORMAT_R16_UNORM:
		return DXGI_FORMAT_R16_UNORM;
	case RHITextureFormat::FORMAT_R16_UINT:
		return DXGI_FORMAT_R16_UINT;
	case RHITextureFormat::FORMAT_R16_SNORM:
		return DXGI_FORMAT_R16_SNORM;
	case RHITextureFormat::FORMAT_R16_SINT:
		return DXGI_FORMAT_R16_SINT;
	case RHITextureFormat::FORMAT_R8_TYPELESS:
		return DXGI_FORMAT_R8_TYPELESS;
	case RHITextureFormat::FORMAT_R8_UNORM:
		return DXGI_FORMAT_R8_UNORM;
	case RHITextureFormat::FORMAT_R8_UINT:
		return DXGI_FORMAT_R8_UINT;
	case RHITextureFormat::FORMAT_R8_SNORM:
		return DXGI_FORMAT_R8_SNORM;
	case RHITextureFormat::FORMAT_R8_SINT:
		return DXGI_FORMAT_R8_SINT;
	case RHITextureFormat::FORMAT_A8_UNORM:
		return DXGI_FORMAT_A8_UNORM;
	case RHITextureFormat::FORMAT_R1_UNORM:
		return DXGI_FORMAT_R1_UNORM;
	case RHITextureFormat::FORMAT_R9G9B9E5_SHAREDEXP:
		return DXGI_FORMAT_R9G9B9E5_SHAREDEXP;
	case RHITextureFormat::FORMAT_R8G8_B8G8_UNORM:
		return DXGI_FORMAT_R8G8_B8G8_UNORM;
	case RHITextureFormat::FORMAT_G8R8_G8B8_UNORM:
		return DXGI_FORMAT_G8R8_G8B8_UNORM;
	case RHITextureFormat::FORMAT_BC1_TYPELESS:
		return DXGI_FORMAT_BC1_TYPELESS;
	case RHITextureFormat::FORMAT_BC1_UNORM:
		return DXGI_FORMAT_BC1_UNORM;
	case RHITextureFormat::FORMAT_BC1_UNORM_SRGB:
		return DXGI_FORMAT_BC1_UNORM_SRGB;
	case RHITextureFormat::FORMAT_BC2_TYPELESS:
		return DXGI_FORMAT_BC2_TYPELESS;
	case RHITextureFormat::FORMAT_BC2_UNORM:
		return DXGI_FORMAT_BC2_UNORM;
	case RHITextureFormat::FORMAT_BC2_UNORM_SRGB:
		return DXGI_FORMAT_BC2_UNORM_SRGB;
	case RHITextureFormat::FORMAT_BC3_TYPELESS:
		return DXGI_FORMAT_BC3_TYPELESS;
	case RHITextureFormat::FORMAT_BC3_UNORM:
		return DXGI_FORMAT_BC3_UNORM;
	case RHITextureFormat::FORMAT_BC3_UNORM_SRGB:
		return DXGI_FORMAT_BC3_UNORM_SRGB;
	case RHITextureFormat::FORMAT_BC4_TYPELESS:
		return DXGI_FORMAT_BC4_TYPELESS;
	case RHITextureFormat::FORMAT_BC4_UNORM:
		return DXGI_FORMAT_BC4_UNORM;
	case RHITextureFormat::FORMAT_BC4_SNORM:
		return DXGI_FORMAT_BC4_SNORM;
	case RHITextureFormat::FORMAT_BC5_TYPELESS:
		return DXGI_FORMAT_BC5_TYPELESS;
	case RHITextureFormat::FORMAT_BC5_UNORM:
		return DXGI_FORMAT_BC5_UNORM;
	case RHITextureFormat::FORMAT_BC5_SNORM:
		return DXGI_FORMAT_BC5_SNORM;
	case RHITextureFormat::FORMAT_B5G6R5_UNORM:
		return DXGI_FORMAT_B5G6R5_UNORM;
	case RHITextureFormat::FORMAT_B5G5R5A1_UNORM:
		return DXGI_FORMAT_B5G5R5A1_UNORM;
	case RHITextureFormat::FORMAT_B8G8R8A8_UNORM:
		return DXGI_FORMAT_B8G8R8A8_UNORM;
	case RHITextureFormat::FORMAT_B8G8R8X8_UNORM:
		return DXGI_FORMAT_B8G8R8X8_UNORM;
	case RHITextureFormat::FORMAT_R10G10B10_XR_BIAS_A2_UNORM:
		return DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM;
	case RHITextureFormat::FORMAT_B8G8R8A8_TYPELESS:
		return DXGI_FORMAT_B8G8R8A8_TYPELESS;
	case RHITextureFormat::FORMAT_B8G8R8A8_UNORM_SRGB:
		return DXGI_FORMAT_B8G8R8A8_UNORM_SRGB;
	case RHITextureFormat::FORMAT_B8G8R8X8_TYPELESS:
		return DXGI_FORMAT_B8G8R8X8_TYPELESS;
	case RHITextureFormat::FORMAT_B8G8R8X8_UNORM_SRGB:
		return DXGI_FORMAT_B8G8R8X8_UNORM_SRGB;
	case RHITextureFormat::FORMAT_BC6H_TYPELESS:
		return DXGI_FORMAT_BC6H_TYPELESS;
	case RHITextureFormat::FORMAT_BC6H_UF16:
		return DXGI_FORMAT_BC6H_UF16;
	case RHITextureFormat::FORMAT_BC6H_SF16:
		return DXGI_FORMAT_BC6H_SF16;
	case RHITextureFormat::FORMAT_BC7_TYPELESS:
		return DXGI_FORMAT_BC7_TYPELESS;
	case RHITextureFormat::FORMAT_BC7_UNORM:
		return DXGI_FORMAT_BC7_UNORM;
	case RHITextureFormat::FORMAT_BC7_UNORM_SRGB:
		return DXGI_FORMAT_BC7_UNORM_SRGB;
	case RHITextureFormat::FORMAT_AYUV:
		return DXGI_FORMAT_AYUV;
	case RHITextureFormat::FORMAT_Y410:
		return DXGI_FORMAT_Y410;
	case RHITextureFormat::FORMAT_Y416:
		return DXGI_FORMAT_Y416;
	case RHITextureFormat::FORMAT_NV12:
		return DXGI_FORMAT_NV12;
	case RHITextureFormat::FORMAT_P010:
		return DXGI_FORMAT_P010;
	case RHITextureFormat::FORMAT_P016:
		return DXGI_FORMAT_P016;
	case RHITextureFormat::FORMAT_420_OPAQUE:
		return DXGI_FORMAT_420_OPAQUE;
	case RHITextureFormat::FORMAT_YUY2:
		return DXGI_FORMAT_YUY2;
	case RHITextureFormat::FORMAT_Y210:
		return DXGI_FORMAT_Y210;
	case RHITextureFormat::FORMAT_Y216:
		return DXGI_FORMAT_Y216;
	case RHITextureFormat::FORMAT_NV11:
		return DXGI_FORMAT_NV11;
	case RHITextureFormat::FORMAT_AI44:
		return DXGI_FORMAT_AI44;
	case RHITextureFormat::FORMAT_IA44:
		return DXGI_FORMAT_IA44;
	case RHITextureFormat::FORMAT_P8:
		return DXGI_FORMAT_P8;
	case RHITextureFormat::FORMAT_A8P8:
		return DXGI_FORMAT_A8P8;
	case RHITextureFormat::FORMAT_B4G4R4A4_UNORM:
		return DXGI_FORMAT_B4G4R4A4_UNORM;
	case RHITextureFormat::FORMAT_P208:
		return DXGI_FORMAT_P208;
	case RHITextureFormat::FORMAT_V208:
		return DXGI_FORMAT_V208;
	case RHITextureFormat::FORMAT_V408:
		return DXGI_FORMAT_V408;
	case RHITextureFormat::FORMAT_FORCE_UINT:
		return DXGI_FORMAT_FORCE_UINT;
	default:
		return DXGI_FORMAT_FORCE_UINT;
	}
}

RHITextureFormat RENDER_API GetGraphicFormatFromDx(const DXGI_FORMAT& graphic_format)
{
	switch (graphic_format)
	{
	case DXGI_FORMAT_UNKNOWN:
		return RHITextureFormat::FORMAT_UNKNOWN;
	case DXGI_FORMAT_R32G32B32A32_TYPELESS:
		return RHITextureFormat::FORMAT_R32G32B32A32_TYPELESS;
	case DXGI_FORMAT_R32G32B32A32_FLOAT:
		return RHITextureFormat::FORMAT_R32G32B32A32_FLOAT;
	case DXGI_FORMAT_R32G32B32A32_UINT:
		return RHITextureFormat::FORMAT_R32G32B32A32_UINT;
	case DXGI_FORMAT_R32G32B32A32_SINT:
		return RHITextureFormat::FORMAT_R32G32B32A32_SINT;
	case DXGI_FORMAT_R32G32B32_TYPELESS:
		return RHITextureFormat::FORMAT_R32G32B32_TYPELESS;
	case DXGI_FORMAT_R32G32B32_FLOAT:
		return RHITextureFormat::FORMAT_R32G32B32_FLOAT;
	case DXGI_FORMAT_R32G32B32_UINT:
		return RHITextureFormat::FORMAT_R32G32B32_UINT;
	case DXGI_FORMAT_R32G32B32_SINT:
		return RHITextureFormat::FORMAT_R32G32B32_SINT;
	case DXGI_FORMAT_R16G16B16A16_TYPELESS:
		return RHITextureFormat::FORMAT_R16G16B16A16_TYPELESS;
	case DXGI_FORMAT_R16G16B16A16_FLOAT:
		return RHITextureFormat::FORMAT_R16G16B16A16_FLOAT;
	case DXGI_FORMAT_R16G16B16A16_UNORM:
		return RHITextureFormat::FORMAT_R16G16B16A16_UNORM;
	case DXGI_FORMAT_R16G16B16A16_UINT:
		return RHITextureFormat::FORMAT_R16G16B16A16_UINT;
	case DXGI_FORMAT_R16G16B16A16_SNORM:
		return RHITextureFormat::FORMAT_R16G16B16A16_SNORM;
	case DXGI_FORMAT_R16G16B16A16_SINT:
		return RHITextureFormat::FORMAT_R16G16B16A16_SINT;
	case DXGI_FORMAT_R32G32_TYPELESS:
		return RHITextureFormat::FORMAT_R32G32_TYPELESS;
	case DXGI_FORMAT_R32G32_FLOAT:
		return RHITextureFormat::FORMAT_R32G32_FLOAT;
	case DXGI_FORMAT_R32G32_UINT:
		return RHITextureFormat::FORMAT_R32G32_UINT;
	case DXGI_FORMAT_R32G32_SINT:
		return RHITextureFormat::FORMAT_R32G32_SINT;
	case DXGI_FORMAT_R32G8X24_TYPELESS:
		return RHITextureFormat::FORMAT_R32G8X24_TYPELESS;
	case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
		return RHITextureFormat::FORMAT_D32_FLOAT_S8X24_UINT;
	case DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS:
		return RHITextureFormat::FORMAT_R32_FLOAT_X8X24_TYPELESS;
	case DXGI_FORMAT_X32_TYPELESS_G8X24_UINT:
		return RHITextureFormat::FORMAT_X32_TYPELESS_G8X24_UINT;
	case DXGI_FORMAT_R10G10B10A2_TYPELESS:
		return RHITextureFormat::FORMAT_R10G10B10A2_TYPELESS;
	case DXGI_FORMAT_R10G10B10A2_UNORM:
		return RHITextureFormat::FORMAT_R10G10B10A2_UNORM;
	case DXGI_FORMAT_R10G10B10A2_UINT:
		return RHITextureFormat::FORMAT_R10G10B10A2_UINT;
	case DXGI_FORMAT_R11G11B10_FLOAT:
		return RHITextureFormat::FORMAT_R11G11B10_FLOAT;
	case DXGI_FORMAT_R8G8B8A8_TYPELESS:
		return RHITextureFormat::FORMAT_R8G8B8A8_TYPELESS;
	case DXGI_FORMAT_R8G8B8A8_UNORM:
		return RHITextureFormat::FORMAT_R8G8BB8A8_UNORM;
	case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
		return RHITextureFormat::FORMAT_R8G8B8A8_UNORM_SRGB;
	case DXGI_FORMAT_R8G8B8A8_UINT:
		return RHITextureFormat::FORMAT_R8G8B8A8_UINT;
	case DXGI_FORMAT_R8G8B8A8_SNORM:
		return RHITextureFormat::FORMAT_R8G8B8A8_SNORM;
	case DXGI_FORMAT_R8G8B8A8_SINT:
		return RHITextureFormat::FORMAT_R8G8B8A8_SINT;
	case DXGI_FORMAT_R16G16_TYPELESS:
		return RHITextureFormat::FORMAT_R16G16_TYPELESS;
	case DXGI_FORMAT_R16G16_FLOAT:
		return RHITextureFormat::FORMAT_R16G16_FLOAT;
	case DXGI_FORMAT_R16G16_UNORM:
		return RHITextureFormat::FORMAT_R16G16_UNORM;
	case DXGI_FORMAT_R16G16_UINT:
		return RHITextureFormat::FORMAT_R16G16_UINT;
	case DXGI_FORMAT_R16G16_SNORM:
		return RHITextureFormat::FORMAT_R16G16_SNORM;
	case DXGI_FORMAT_R16G16_SINT:
		return RHITextureFormat::FORMAT_R16G16_SINT;
	case DXGI_FORMAT_R32_TYPELESS:
		return RHITextureFormat::FORMAT_R32_TYPELESS;
	case DXGI_FORMAT_D32_FLOAT:
		return RHITextureFormat::FORMAT_D32_FLOAT;
	case DXGI_FORMAT_R32_FLOAT:
		return RHITextureFormat::FORMAT_R32_FLOAT;
	case DXGI_FORMAT_R32_UINT:
		return RHITextureFormat::FORMAT_R32_UINT;
	case DXGI_FORMAT_R32_SINT:
		return RHITextureFormat::FORMAT_R32_SINT;
	case DXGI_FORMAT_R24G8_TYPELESS:
		return RHITextureFormat::FORMAT_R24G8_TYPELESS;
	case DXGI_FORMAT_D24_UNORM_S8_UINT:
		return RHITextureFormat::FORMAT_D24_UNORM_S8_UINT;
	case DXGI_FORMAT_R24_UNORM_X8_TYPELESS:
		return RHITextureFormat::FORMAT_R24_UNORM_X8_TYPELESS;
	case DXGI_FORMAT_X24_TYPELESS_G8_UINT:
		return RHITextureFormat::FORMAT_X24_TYPELESS_G8_UINT;
	case DXGI_FORMAT_R8G8_TYPELESS:
		return RHITextureFormat::FORMAT_R8G8_TYPELESS;
	case DXGI_FORMAT_R8G8_UNORM:
		return RHITextureFormat::FORMAT_R8G8_UNORM;
	case DXGI_FORMAT_R8G8_UINT:
		return RHITextureFormat::FORMAT_R8G8_UINT;
	case DXGI_FORMAT_R8G8_SNORM:
		return RHITextureFormat::FORMAT_R8G8_SNORM;
	case DXGI_FORMAT_R8G8_SINT:
		return RHITextureFormat::FORMAT_R8G8_SINT;
	case DXGI_FORMAT_R16_TYPELESS:
		return RHITextureFormat::FORMAT_R16_TYPELESS;
	case DXGI_FORMAT_R16_FLOAT:
		return RHITextureFormat::FORMAT_R16_FLOAT;
	case DXGI_FORMAT_D16_UNORM:
		return RHITextureFormat::FORMAT_D16_UNORM;
	case DXGI_FORMAT_R16_UNORM:
		return RHITextureFormat::FORMAT_R16_UNORM;
	case DXGI_FORMAT_R16_UINT:
		return RHITextureFormat::FORMAT_R16_UINT;
	case DXGI_FORMAT_R16_SNORM:
		return RHITextureFormat::FORMAT_R16_SNORM;
	case DXGI_FORMAT_R16_SINT:
		return RHITextureFormat::FORMAT_R16_SINT;
	case DXGI_FORMAT_R8_TYPELESS:
		return RHITextureFormat::FORMAT_R8_TYPELESS;
	case DXGI_FORMAT_R8_UNORM:
		return RHITextureFormat::FORMAT_R8_UNORM;
	case DXGI_FORMAT_R8_UINT:
		return RHITextureFormat::FORMAT_R8_UINT;
	case DXGI_FORMAT_R8_SNORM:
		return RHITextureFormat::FORMAT_R8_SNORM;
	case DXGI_FORMAT_R8_SINT:
		return RHITextureFormat::FORMAT_R8_SINT;
	case DXGI_FORMAT_A8_UNORM:
		return RHITextureFormat::FORMAT_A8_UNORM;
	case DXGI_FORMAT_R1_UNORM:
		return RHITextureFormat::FORMAT_R1_UNORM;
	case DXGI_FORMAT_R9G9B9E5_SHAREDEXP:
		return RHITextureFormat::FORMAT_R9G9B9E5_SHAREDEXP;
	case DXGI_FORMAT_R8G8_B8G8_UNORM:
		return RHITextureFormat::FORMAT_R8G8_B8G8_UNORM;
	case DXGI_FORMAT_G8R8_G8B8_UNORM:
		return RHITextureFormat::FORMAT_G8R8_G8B8_UNORM;
	case DXGI_FORMAT_BC1_TYPELESS:
		return RHITextureFormat::FORMAT_BC1_TYPELESS;
	case DXGI_FORMAT_BC1_UNORM:
		return RHITextureFormat::FORMAT_BC1_UNORM;
	case DXGI_FORMAT_BC1_UNORM_SRGB:
		return RHITextureFormat::FORMAT_BC1_UNORM_SRGB;
	case DXGI_FORMAT_BC2_TYPELESS:
		return RHITextureFormat::FORMAT_BC2_TYPELESS;
	case DXGI_FORMAT_BC2_UNORM:
		return RHITextureFormat::FORMAT_BC2_UNORM;
	case DXGI_FORMAT_BC2_UNORM_SRGB:
		return RHITextureFormat::FORMAT_BC2_UNORM_SRGB;
	case DXGI_FORMAT_BC3_TYPELESS:
		return RHITextureFormat::FORMAT_BC3_TYPELESS;
	case DXGI_FORMAT_BC3_UNORM:
		return RHITextureFormat::FORMAT_BC3_UNORM;
	case DXGI_FORMAT_BC3_UNORM_SRGB:
		return RHITextureFormat::FORMAT_BC3_UNORM_SRGB;
	case DXGI_FORMAT_BC4_TYPELESS:
		return RHITextureFormat::FORMAT_BC4_TYPELESS;
	case DXGI_FORMAT_BC4_UNORM:
		return RHITextureFormat::FORMAT_BC4_UNORM;
	case DXGI_FORMAT_BC4_SNORM:
		return RHITextureFormat::FORMAT_BC4_SNORM;
	case DXGI_FORMAT_BC5_TYPELESS:
		return RHITextureFormat::FORMAT_BC5_TYPELESS;
	case DXGI_FORMAT_BC5_UNORM:
		return RHITextureFormat::FORMAT_BC5_UNORM;
	case DXGI_FORMAT_BC5_SNORM:
		return RHITextureFormat::FORMAT_BC5_SNORM;
	case DXGI_FORMAT_B5G6R5_UNORM:
		return RHITextureFormat::FORMAT_B5G6R5_UNORM;
	case DXGI_FORMAT_B5G5R5A1_UNORM:
		return RHITextureFormat::FORMAT_B5G5R5A1_UNORM;
	case DXGI_FORMAT_B8G8R8A8_UNORM:
		return RHITextureFormat::FORMAT_B8G8R8A8_UNORM;
	case DXGI_FORMAT_B8G8R8X8_UNORM:
		return RHITextureFormat::FORMAT_B8G8R8X8_UNORM;
	case DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM:
		return RHITextureFormat::FORMAT_R10G10B10_XR_BIAS_A2_UNORM;
	case DXGI_FORMAT_B8G8R8A8_TYPELESS:
		return RHITextureFormat::FORMAT_B8G8R8A8_TYPELESS;
	case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
		return RHITextureFormat::FORMAT_B8G8R8A8_UNORM_SRGB;
	case DXGI_FORMAT_B8G8R8X8_TYPELESS:
		return RHITextureFormat::FORMAT_B8G8R8X8_TYPELESS;
	case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:
		return RHITextureFormat::FORMAT_B8G8R8X8_UNORM_SRGB;
	case DXGI_FORMAT_BC6H_TYPELESS:
		return RHITextureFormat::FORMAT_BC6H_TYPELESS;
	case DXGI_FORMAT_BC6H_UF16:
		return RHITextureFormat::FORMAT_BC6H_UF16;
	case DXGI_FORMAT_BC6H_SF16:
		return RHITextureFormat::FORMAT_BC6H_SF16;
	case DXGI_FORMAT_BC7_TYPELESS:
		return RHITextureFormat::FORMAT_BC7_TYPELESS;
	case DXGI_FORMAT_BC7_UNORM:
		return RHITextureFormat::FORMAT_BC7_UNORM;
	case DXGI_FORMAT_BC7_UNORM_SRGB:
		return RHITextureFormat::FORMAT_BC7_UNORM_SRGB;
	case DXGI_FORMAT_AYUV:
		return RHITextureFormat::FORMAT_AYUV;
	case DXGI_FORMAT_Y410:
		return RHITextureFormat::FORMAT_Y410;
	case DXGI_FORMAT_Y416:
		return RHITextureFormat::FORMAT_Y416;
	case DXGI_FORMAT_NV12:
		return RHITextureFormat::FORMAT_NV12;
	case DXGI_FORMAT_P010:
		return RHITextureFormat::FORMAT_P010;
	case DXGI_FORMAT_P016:
		return RHITextureFormat::FORMAT_P016;
	case DXGI_FORMAT_420_OPAQUE:
		return RHITextureFormat::FORMAT_420_OPAQUE;
	case DXGI_FORMAT_YUY2:
		return RHITextureFormat::FORMAT_YUY2;
	case DXGI_FORMAT_Y210:
		return RHITextureFormat::FORMAT_Y210;
	case DXGI_FORMAT_Y216:
		return RHITextureFormat::FORMAT_Y216;
	case DXGI_FORMAT_NV11:
		return RHITextureFormat::FORMAT_NV11;
	case DXGI_FORMAT_AI44:
		return RHITextureFormat::FORMAT_AI44;
	case DXGI_FORMAT_IA44:
		return RHITextureFormat::FORMAT_IA44;
	case DXGI_FORMAT_P8:
		return RHITextureFormat::FORMAT_P8;
	case DXGI_FORMAT_A8P8:
		return RHITextureFormat::FORMAT_A8P8;
	case DXGI_FORMAT_B4G4R4A4_UNORM:
		return RHITextureFormat::FORMAT_B4G4R4A4_UNORM;
	case DXGI_FORMAT_P208:
		return RHITextureFormat::FORMAT_P208;
	case DXGI_FORMAT_V208:
		return RHITextureFormat::FORMAT_V208;
	case DXGI_FORMAT_V408:
		return RHITextureFormat::FORMAT_V408;
		// 			case DXGI_FORMAT_SAMPLER_FEEDBACK_MIN_MIP_OPAQUE:
		// 				return LunarGraphicFormat::FORMAT_SAMPLER_FEEDBACK_MIN_MIP_OPAQUE;
		// 			case DXGI_FORMAT_SAMPLER_FEEDBACK_MIP_REGION_USED_OPAQUE:
		// 				return LunarGraphicFormat::FORMAT_SAMPLER_FEEDBACK_MIP_REGION_USED_OPAQUE;
	case DXGI_FORMAT_FORCE_UINT:
		return RHITextureFormat::FORMAT_FORCE_UINT;
	default:
		return RHITextureFormat::FORMAT_FORCE_UINT;
	}
}

D3D12_RESOURCE_DIMENSION RENDER_API GetResourceDimension(const RHIResDimension& res_dimension)
{
	switch (res_dimension)
	{
	case RHIResDimension::Buffer:
		return D3D12_RESOURCE_DIMENSION::D3D12_RESOURCE_DIMENSION_BUFFER;
	case RHIResDimension::Texture1D:
		return D3D12_RESOURCE_DIMENSION::D3D12_RESOURCE_DIMENSION_TEXTURE1D;
	case RHIResDimension::Texture2D:
		return D3D12_RESOURCE_DIMENSION::D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	case RHIResDimension::Texture3D:
		return D3D12_RESOURCE_DIMENSION::D3D12_RESOURCE_DIMENSION_TEXTURE3D;
	case RHIResDimension::Unknown:
		return D3D12_RESOURCE_DIMENSION::D3D12_RESOURCE_DIMENSION_UNKNOWN;
	default:
		return D3D12_RESOURCE_DIMENSION::D3D12_RESOURCE_DIMENSION_UNKNOWN;
	}
}

RHIResDimension RENDER_API GetResourceDimensionDx(const D3D12_RESOURCE_DIMENSION& res_dimension)
{
	switch (res_dimension)
	{
	case D3D12_RESOURCE_DIMENSION::D3D12_RESOURCE_DIMENSION_BUFFER:
		return RHIResDimension::Buffer;
	case D3D12_RESOURCE_DIMENSION::D3D12_RESOURCE_DIMENSION_TEXTURE1D:
		return RHIResDimension::Texture1D;
	case D3D12_RESOURCE_DIMENSION::D3D12_RESOURCE_DIMENSION_TEXTURE2D:
		return RHIResDimension::Texture2D;
	case D3D12_RESOURCE_DIMENSION::D3D12_RESOURCE_DIMENSION_TEXTURE3D:
		return RHIResDimension::Texture3D;
	case D3D12_RESOURCE_DIMENSION::D3D12_RESOURCE_DIMENSION_UNKNOWN:
		return RHIResDimension::Unknown;
	default:
		return RHIResDimension::Unknown;
	}
}

D3D12_TEXTURE_LAYOUT RENDER_API GetTextureLayout(const RHITextureLayout& texture_layout)
{
	switch (texture_layout)
	{
	case RHITextureLayout::TEXTURE_LAYOUT_64KB_STANDARD_SWIZZLE:
		return D3D12_TEXTURE_LAYOUT::D3D12_TEXTURE_LAYOUT_64KB_STANDARD_SWIZZLE;
	case RHITextureLayout::TEXTURE_LAYOUT_64KB_UNDEFINED_SWIZZLE:
		return D3D12_TEXTURE_LAYOUT::D3D12_TEXTURE_LAYOUT_64KB_UNDEFINED_SWIZZLE;
	case RHITextureLayout::TEXTURE_LAYOUT_ROW_MAJOR:
		return D3D12_TEXTURE_LAYOUT::D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	case RHITextureLayout::LayoutUnknown:
		return D3D12_TEXTURE_LAYOUT::D3D12_TEXTURE_LAYOUT_UNKNOWN;
	default:
		return D3D12_TEXTURE_LAYOUT::D3D12_TEXTURE_LAYOUT_UNKNOWN;
	}
}

RHITextureLayout RENDER_API GetTextureLayoutDx(const D3D12_TEXTURE_LAYOUT& texture_layout)
{
	switch (texture_layout)
	{
	case D3D12_TEXTURE_LAYOUT::D3D12_TEXTURE_LAYOUT_64KB_STANDARD_SWIZZLE:
		return RHITextureLayout::TEXTURE_LAYOUT_64KB_STANDARD_SWIZZLE;
	case D3D12_TEXTURE_LAYOUT::D3D12_TEXTURE_LAYOUT_64KB_UNDEFINED_SWIZZLE:
		return RHITextureLayout::TEXTURE_LAYOUT_64KB_UNDEFINED_SWIZZLE;
	case D3D12_TEXTURE_LAYOUT::D3D12_TEXTURE_LAYOUT_ROW_MAJOR:
		return RHITextureLayout::TEXTURE_LAYOUT_ROW_MAJOR;
	case D3D12_TEXTURE_LAYOUT::D3D12_TEXTURE_LAYOUT_UNKNOWN:
		return RHITextureLayout::LayoutUnknown;
	default:
		return RHITextureLayout::LayoutUnknown;
	}
}

D3D12_RESOURCE_FLAGS RENDER_API GetGpuResourceFlag(const RHIResourceUsage& gpu_res_flag)
{
	switch (gpu_res_flag)
	{
	case RHIResourceUsage::RESOURCE_FLAG_NONE:
		return D3D12_RESOURCE_FLAG_NONE;
	case RHIResourceUsage::RESOURCE_FLAG_ALLOW_RENDER_TARGET:
		return D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
	case RHIResourceUsage::RESOURCE_FLAG_ALLOW_DEPTH_STENCIL:
		return D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
	case RHIResourceUsage::RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS:
		return D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
	case RHIResourceUsage::RESOURCE_FLAG_DENY_SHADER_RESOURCE:
		return D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE;
	case RHIResourceUsage::RESOURCE_FLAG_ALLOW_CROSS_ADAPTER:
		return D3D12_RESOURCE_FLAG_ALLOW_CROSS_ADAPTER;
	case RHIResourceUsage::RESOURCE_FLAG_ALLOW_SIMULTANEOUS_ACCESS:
		return D3D12_RESOURCE_FLAG_ALLOW_SIMULTANEOUS_ACCESS;
	case RHIResourceUsage::RESOURCE_FLAG_VIDEO_DECODE_REFERENCE_ONLY:
		return D3D12_RESOURCE_FLAG_VIDEO_DECODE_REFERENCE_ONLY;
	default:
		return D3D12_RESOURCE_FLAG_NONE;
	}
}

RHIResourceUsage RENDER_API GetGpuResourceFlagDx(const D3D12_RESOURCE_FLAGS& gpu_res_flag)
{
	switch (gpu_res_flag)
	{
	case D3D12_RESOURCE_FLAG_NONE:
		return RHIResourceUsage::RESOURCE_FLAG_NONE;
	case D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET:
		return RHIResourceUsage::RESOURCE_FLAG_ALLOW_RENDER_TARGET;
	case D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL:
		return RHIResourceUsage::RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
	case D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS:
		return RHIResourceUsage::RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
	case D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE:
		return RHIResourceUsage::RESOURCE_FLAG_DENY_SHADER_RESOURCE;
	case D3D12_RESOURCE_FLAG_ALLOW_CROSS_ADAPTER:
		return RHIResourceUsage::RESOURCE_FLAG_ALLOW_CROSS_ADAPTER;
	case D3D12_RESOURCE_FLAG_ALLOW_SIMULTANEOUS_ACCESS:
		return RHIResourceUsage::RESOURCE_FLAG_ALLOW_SIMULTANEOUS_ACCESS;
	case D3D12_RESOURCE_FLAG_VIDEO_DECODE_REFERENCE_ONLY:
		return RHIResourceUsage::RESOURCE_FLAG_VIDEO_DECODE_REFERENCE_ONLY;
	default:
		return RHIResourceUsage::RESOURCE_FLAG_NONE;
	}
}

D3D12_RESOURCE_DESC RENDER_API GetResourceDesc(const RHIResDesc& res_desc)
{
	D3D12_RESOURCE_DESC desc_out = {};
	desc_out.Dimension = GetResourceDimension(res_desc.Dimension);
	desc_out.Alignment = res_desc.Alignment;
	desc_out.Width = res_desc.Width;
	desc_out.Height = res_desc.Height;
	desc_out.DepthOrArraySize = res_desc.DepthOrArraySize;
	desc_out.MipLevels = res_desc.MipLevels;
	desc_out.Format = GetGraphicFormat(res_desc.Format);
	desc_out.SampleDesc.Count = res_desc.SampleDesc.Count;
	desc_out.SampleDesc.Quality = res_desc.SampleDesc.Quality;
	desc_out.Layout = GetTextureLayout(res_desc.Layout);
	desc_out.Flags = GetGpuResourceFlag(res_desc.mUsage);
	return desc_out;
}

RHIResDesc RENDER_API GetResourceDescDx(const D3D12_RESOURCE_DESC& res_desc)
{
	RHIResDesc desc_out = {};
	desc_out.Dimension = GetResourceDimensionDx(res_desc.Dimension);
	desc_out.Alignment = res_desc.Alignment;
	desc_out.Width = res_desc.Width;
	desc_out.Height = res_desc.Height;
	desc_out.DepthOrArraySize = res_desc.DepthOrArraySize;
	desc_out.MipLevels = res_desc.MipLevels;
	desc_out.Format = GetGraphicFormatFromDx(res_desc.Format);
	desc_out.SampleDesc.Count = res_desc.SampleDesc.Count;
	desc_out.SampleDesc.Quality = res_desc.SampleDesc.Quality;
	desc_out.Layout = GetTextureLayoutDx(res_desc.Layout);
	desc_out.mUsage = GetGpuResourceFlagDx(res_desc.Flags);
	return desc_out;
}

D3D12_HEAP_TYPE RENDER_API GetHeapType(const RHIHeapType& heap_type)
{
	switch (heap_type)
	{
	case RHIHeapType::Custom:
		return D3D12_HEAP_TYPE::D3D12_HEAP_TYPE_CUSTOM;
	case RHIHeapType::Default:
		return D3D12_HEAP_TYPE::D3D12_HEAP_TYPE_DEFAULT;
	case RHIHeapType::Readback:
		return D3D12_HEAP_TYPE::D3D12_HEAP_TYPE_READBACK;
	case RHIHeapType::Upload:
		return D3D12_HEAP_TYPE::D3D12_HEAP_TYPE_UPLOAD;
	default:
		return D3D12_HEAP_TYPE::D3D12_HEAP_TYPE_CUSTOM;
	}
}

D3D12_HEAP_FLAGS RENDER_API GetHeapFlag(const RHIHeapFlag& heap_flag)
{
	switch (heap_flag)
	{
	case RHIHeapFlag::HEAP_FLAG_NONE:
		return D3D12_HEAP_FLAG_NONE;
	case RHIHeapFlag::HEAP_FLAG_SHARED:
		return D3D12_HEAP_FLAG_SHARED;
	case RHIHeapFlag::HEAP_FLAG_DENY_BUFFERS:
		return D3D12_HEAP_FLAG_DENY_BUFFERS;
	case RHIHeapFlag::HEAP_FLAG_ALLOW_DISPLAY:
		return D3D12_HEAP_FLAG_ALLOW_DISPLAY;
	case RHIHeapFlag::HEAP_FLAG_SHARED_CROSS_ADAPTER:
		return D3D12_HEAP_FLAG_SHARED_CROSS_ADAPTER;
	case RHIHeapFlag::HEAP_FLAG_DENY_RT_DS_TEXTURES:
		return D3D12_HEAP_FLAG_DENY_RT_DS_TEXTURES;
	case RHIHeapFlag::HEAP_FLAG_DENY_NON_RT_DS_TEXTURES:
		return D3D12_HEAP_FLAG_DENY_NON_RT_DS_TEXTURES;
	case RHIHeapFlag::HEAP_FLAG_HARDWARE_PROTECTED:
		return D3D12_HEAP_FLAG_HARDWARE_PROTECTED;
	case RHIHeapFlag::HEAP_FLAG_ALLOW_WRITE_WATCH:
		return D3D12_HEAP_FLAG_ALLOW_WRITE_WATCH;
	case RHIHeapFlag::HEAP_FLAG_ALLOW_SHADER_ATOMICS:
		return D3D12_HEAP_FLAG_ALLOW_SHADER_ATOMICS;
		// 			case RHIHeapFlag::HEAP_FLAG_CREATE_NOT_RESIDENT:
		// 				return D3D12_HEAP_FLAG_CREATE_NOT_RESIDENT;
		// 			case RHIHeapFlag::HEAP_FLAG_CREATE_NOT_ZEROED:
		// 				return D3D12_HEAP_FLAG_CREATE_NOT_ZEROED;
	case RHIHeapFlag::HEAP_FLAG_ALLOW_ONLY_BUFFERS:
		return D3D12_HEAP_FLAG_ALLOW_ONLY_BUFFERS;
	case RHIHeapFlag::HEAP_FLAG_ALLOW_ONLY_NON_RT_DS_TEXTURES:
		return D3D12_HEAP_FLAG_ALLOW_ONLY_NON_RT_DS_TEXTURES;
	case RHIHeapFlag::HEAP_FLAG_ALLOW_ONLY_RT_DS_TEXTURES:
		return D3D12_HEAP_FLAG_ALLOW_ONLY_RT_DS_TEXTURES;
	default:
		return D3D12_HEAP_FLAG_NONE;
	}
}


D3D12_BUFFER_SRV_FLAGS RENDER_API GetBufferSrvFlags(LBufferSrvFlag srv_dimention)
{
	switch (srv_dimention)
	{
	case LBufferSrvFlag::LUNA_BUFFER_SRV_FLAG_NONE:
		return D3D12_BUFFER_SRV_FLAGS::D3D12_BUFFER_SRV_FLAG_NONE;
		break;
	case LBufferSrvFlag::LUNA_BUFFER_SRV_FLAG_RAW:
		return D3D12_BUFFER_SRV_FLAGS::D3D12_BUFFER_SRV_FLAG_RAW;
		break;
	default:
		break;
	}
	return D3D12_BUFFER_SRV_FLAGS::D3D12_BUFFER_SRV_FLAG_NONE;
}

D3D12_SRV_DIMENSION RENDER_API GetSrvDimention(RHIResDimension srv_dimention)
{
	switch (srv_dimention)
	{
	case RHIResDimension::Buffer:
		return D3D12_SRV_DIMENSION::D3D12_SRV_DIMENSION_BUFFER;
		break;
	case RHIResDimension::Texture1D:
		return D3D12_SRV_DIMENSION::D3D12_SRV_DIMENSION_TEXTURE1D;
		break;
	case RHIResDimension::Texture2D:
		return D3D12_SRV_DIMENSION::D3D12_SRV_DIMENSION_TEXTURE2D;
		break;
	case RHIResDimension::Texture3D:
		return D3D12_SRV_DIMENSION::D3D12_SRV_DIMENSION_TEXTURE3D;
		break;
	default:
		assert(false);
		break;
	}
	return D3D12_SRV_DIMENSION::D3D12_SRV_DIMENSION_UNKNOWN;
}

D3D12_SHADER_RESOURCE_VIEW_DESC RENDER_API GetShaderResourceViewDesc(const LShaderResourceViewDesc& srv_desc)
{
	D3D12_SHADER_RESOURCE_VIEW_DESC desc_out = {};
	desc_out.Format = GetGraphicFormat(srv_desc.Format);
	desc_out.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	switch (srv_desc.ViewDimension)
	{
	case RHISrvDimension::SRV_Unknown:
		break;
	case RHISrvDimension::SRV_Buffer:
		desc_out.Buffer.FirstElement = srv_desc.FirstElement;
		desc_out.Buffer.Flags = GetBufferSrvFlags(srv_desc.Flags);
		desc_out.Buffer.NumElements = srv_desc.NumElements;
		desc_out.Buffer.StructureByteStride = srv_desc.StructureByteStride;
		break;
	case RHISrvDimension::SRV_Texture1D:
		desc_out.Texture1D.MipLevels = srv_desc.MipLevels;
		desc_out.Texture1D.MostDetailedMip = srv_desc.MostDetailedMip;
		desc_out.Texture1D.ResourceMinLODClamp = srv_desc.ResourceMinLODClamp;
		break;
	case RHISrvDimension::SRV_DIMENSION_TEXTURE1DARRAY:
		desc_out.Texture1DArray.ArraySize = srv_desc.ArraySize;
		desc_out.Texture1DArray.FirstArraySlice = srv_desc.FirstArraySlice;
		desc_out.Texture1DArray.MipLevels = srv_desc.MipLevels;
		desc_out.Texture1DArray.MostDetailedMip = srv_desc.MostDetailedMip;
		desc_out.Texture1DArray.ResourceMinLODClamp = srv_desc.ResourceMinLODClamp;
		break;
	case RHISrvDimension::SRV_Texture2D:
		desc_out.Texture2D.MipLevels = srv_desc.MipLevels;
		desc_out.Texture2D.MostDetailedMip = srv_desc.MostDetailedMip;
		desc_out.Texture2D.PlaneSlice = srv_desc.PlaneSlice;
		desc_out.Texture2D.ResourceMinLODClamp = srv_desc.ResourceMinLODClamp;
		break;
	case RHISrvDimension::SRV_DIMENSION_TEXTURE2DARRAY:
		desc_out.Texture2DArray.ArraySize = srv_desc.ArraySize;
		desc_out.Texture2DArray.FirstArraySlice = srv_desc.FirstArraySlice;
		desc_out.Texture2DArray.MipLevels = srv_desc.MipLevels;
		desc_out.Texture2DArray.MostDetailedMip = srv_desc.MostDetailedMip;
		desc_out.Texture2DArray.PlaneSlice = srv_desc.PlaneSlice;
		desc_out.Texture2DArray.ResourceMinLODClamp = srv_desc.ResourceMinLODClamp;
		break;
	case RHISrvDimension::SRV_DIMENSION_TEXTURE2DMS:
		desc_out.Texture2DMS.UnusedField_NothingToDefine = srv_desc.UnusedField_NothingToDefine;
		break;
	case RHISrvDimension::SRV_DIMENSION_TEXTURE2DMSARRAY:
		desc_out.Texture2DMSArray.ArraySize = srv_desc.ArraySize;
		desc_out.Texture2DMSArray.FirstArraySlice = srv_desc.FirstArraySlice;
		break;
	case RHISrvDimension::SRV_DIMENSION_TEXTURE3D:
		desc_out.Texture3D.MipLevels = srv_desc.MipLevels;
		desc_out.Texture3D.MostDetailedMip = srv_desc.MostDetailedMip;
		desc_out.Texture3D.ResourceMinLODClamp = srv_desc.ResourceMinLODClamp;
		break;
	case RHISrvDimension::SRV_TextureCube:
		desc_out.TextureCube.MipLevels = srv_desc.MipLevels;
		desc_out.TextureCube.MostDetailedMip = srv_desc.MostDetailedMip;
		desc_out.TextureCube.ResourceMinLODClamp = srv_desc.ResourceMinLODClamp;
		break;
	case RHISrvDimension::SRV_DIMENSION_TEXTURECUBEARRAY:
		desc_out.TextureCubeArray.First2DArrayFace = srv_desc.First2DArrayFace;
		desc_out.TextureCubeArray.NumCubes = srv_desc.NumCubes;
		desc_out.TextureCubeArray.MipLevels = srv_desc.MipLevels;
		desc_out.TextureCubeArray.MostDetailedMip = srv_desc.MostDetailedMip;
		desc_out.TextureCubeArray.ResourceMinLODClamp = srv_desc.ResourceMinLODClamp;
		break;
	case RHISrvDimension::SRV_DIMENSION_RAYTRACING_ACCELERATION_STRUCTURE:
		desc_out.RaytracingAccelerationStructure.Location = srv_desc.Location;
		break;
	default:
		break;
	}
	return desc_out;
}


D3D12_BUFFER_UAV_FLAGS RENDER_API GetBufferUavFlags(LBufferUavFlag uav_flag)
{
	switch (uav_flag)
	{
	case LBufferUavFlag::LUNA_BUFFER_UAV_FLAG_NONE:
		return D3D12_BUFFER_UAV_FLAGS::D3D12_BUFFER_UAV_FLAG_NONE;
		break;
	case LBufferUavFlag::LUNA_BUFFER_UAV_FLAG_RAW:
		return D3D12_BUFFER_UAV_FLAGS::D3D12_BUFFER_UAV_FLAG_RAW;
		break;
	default:
		break;
	}
	return D3D12_BUFFER_UAV_FLAGS::D3D12_BUFFER_UAV_FLAG_NONE;
}

D3D12_UAV_DIMENSION RENDER_API GetUavDimention(LUAVDimention uav_dimention)
{
	switch (uav_dimention)
	{
	case LUAVDimention::LUNA_UAV_DIMENSION_UNKNOWN:
		return D3D12_UAV_DIMENSION::D3D12_UAV_DIMENSION_UNKNOWN;
		break;
	case LUAVDimention::LUNA_UAV_DIMENSION_BUFFER:
		return D3D12_UAV_DIMENSION::D3D12_UAV_DIMENSION_BUFFER;
		break;
	case LUAVDimention::LUNA_UAV_DIMENSION_TEXTURE1D:
		return D3D12_UAV_DIMENSION::D3D12_UAV_DIMENSION_TEXTURE1D;
		break;
	case LUAVDimention::LUNA_UAV_DIMENSION_TEXTURE1DARRAY:
		return D3D12_UAV_DIMENSION::D3D12_UAV_DIMENSION_TEXTURE1DARRAY;
		break;
	case LUAVDimention::LUNA_UAV_DIMENSION_TEXTURE2D:
		return D3D12_UAV_DIMENSION::D3D12_UAV_DIMENSION_TEXTURE2D;
		break;
	case LUAVDimention::LUNA_UAV_DIMENSION_TEXTURE2DARRAY:
		return D3D12_UAV_DIMENSION::D3D12_UAV_DIMENSION_TEXTURE2DARRAY;
		break;
	case LUAVDimention::LUNA_UAV_DIMENSION_TEXTURE3D:
		return D3D12_UAV_DIMENSION::D3D12_UAV_DIMENSION_TEXTURE3D;
		break;
	default:
		break;
	}
	return D3D12_UAV_DIMENSION::D3D12_UAV_DIMENSION_UNKNOWN;
}

D3D12_UNORDERED_ACCESS_VIEW_DESC RENDER_API GetUnorderedAccessViewDesc(const LUnorderedAccessViewDesc& uav_desc)
{
	D3D12_UNORDERED_ACCESS_VIEW_DESC desc_out = {};
	desc_out.Format = GetGraphicFormat(uav_desc.Format);
	desc_out.ViewDimension = GetUavDimention(uav_desc.ViewDimension);
	switch (uav_desc.ViewDimension)
	{
	case LUAVDimention::LUNA_UAV_DIMENSION_UNKNOWN:
		break;
	case LUAVDimention::LUNA_UAV_DIMENSION_BUFFER:
		desc_out.Buffer.CounterOffsetInBytes = uav_desc.CounterOffsetInBytes;
		desc_out.Buffer.FirstElement = uav_desc.FirstElement;
		desc_out.Buffer.Flags = GetBufferUavFlags(uav_desc.Flags);
		desc_out.Buffer.NumElements = uav_desc.NumElements;
		desc_out.Buffer.StructureByteStride = uav_desc.StructureByteStride;
		break;
	case LUAVDimention::LUNA_UAV_DIMENSION_TEXTURE1D:
		desc_out.Texture1D.MipSlice = uav_desc.MipSlice;
		break;
	case LUAVDimention::LUNA_UAV_DIMENSION_TEXTURE1DARRAY:
		desc_out.Texture1DArray.ArraySize = uav_desc.ArraySize;
		desc_out.Texture1DArray.FirstArraySlice = uav_desc.FirstArraySlice;
		desc_out.Texture1DArray.MipSlice = uav_desc.MipSlice;
		break;
	case LUAVDimention::LUNA_UAV_DIMENSION_TEXTURE2D:
		desc_out.Texture2D.MipSlice = uav_desc.MipSlice;
		desc_out.Texture2D.PlaneSlice = uav_desc.PlaneSlice;
		break;
	case LUAVDimention::LUNA_UAV_DIMENSION_TEXTURE2DARRAY:
		desc_out.Texture2DArray.ArraySize = uav_desc.ArraySize;
		desc_out.Texture2DArray.FirstArraySlice = uav_desc.FirstArraySlice;
		desc_out.Texture2DArray.MipSlice = uav_desc.MipSlice;
		desc_out.Texture2DArray.PlaneSlice = uav_desc.PlaneSlice;
		break;
	case LUAVDimention::LUNA_UAV_DIMENSION_TEXTURE3D:
		desc_out.Texture3D.FirstWSlice = uav_desc.FirstWSlice;
		desc_out.Texture3D.MipSlice = uav_desc.MipSlice;
		desc_out.Texture3D.WSize = uav_desc.WSize;
		break;
	}
	return desc_out;
}


D3D12_RTV_DIMENSION RENDER_API GetRtvDimention(RHIViewDimension rtv_dimention)
{
	switch (rtv_dimention)
	{
	case RHIViewDimension::TextureView2D:
		return D3D12_RTV_DIMENSION::D3D12_RTV_DIMENSION_TEXTURE2D;
		break;
	default:
		assert(false);
		break;
	}
	return D3D12_RTV_DIMENSION::D3D12_RTV_DIMENSION_UNKNOWN;
}

D3D12_RENDER_TARGET_VIEW_DESC RENDER_API GetRenderTargetViewDesc(const LRenderTargetViewDesc& rtv_desc)
{
	D3D12_RENDER_TARGET_VIEW_DESC desc_out = {};
	desc_out.Format = GetGraphicFormat(rtv_desc.Format);
	switch (rtv_desc.ViewDimension)
	{
	case LRTViewDimention::LUNA_RTV_DIMENSION_UNKNOWN:
		break;
	case LRTViewDimention::LUNA_RTV_DIMENSION_BUFFER:
		desc_out.Buffer.FirstElement = rtv_desc.FirstElement;
		desc_out.Buffer.NumElements = rtv_desc.NumElements;
		break;
	case LRTViewDimention::LUNA_RTV_DIMENSION_TEXTURE1D:
		desc_out.Texture1D.MipSlice = rtv_desc.MipSlice;
		break;
	case LRTViewDimention::LUNA_RTV_DIMENSION_TEXTURE1DARRAY:
		desc_out.Texture1DArray.ArraySize = rtv_desc.ArraySize;
		desc_out.Texture1DArray.FirstArraySlice = rtv_desc.FirstArraySlice;
		desc_out.Texture1DArray.MipSlice = rtv_desc.MipSlice;
		break;
	case LRTViewDimention::LUNA_RTV_DIMENSION_TEXTURE2D:
		desc_out.Texture2D.MipSlice = rtv_desc.MipSlice;
		desc_out.Texture2D.PlaneSlice = rtv_desc.PlaneSlice;
		break;
	case LRTViewDimention::LUNA_RTV_DIMENSION_TEXTURE2DARRAY:
		desc_out.Texture2DArray.ArraySize = rtv_desc.ArraySize;
		desc_out.Texture2DArray.FirstArraySlice = rtv_desc.FirstArraySlice;
		desc_out.Texture2DArray.MipSlice = rtv_desc.MipSlice;
		desc_out.Texture2DArray.PlaneSlice = rtv_desc.PlaneSlice;
		break;
	case LRTViewDimention::LUNA_RTV_DIMENSION_TEXTURE2DMS:
		desc_out.Texture2DMS.UnusedField_NothingToDefine = rtv_desc.UnusedField_NothingToDefine;
		break;
	case LRTViewDimention::LUNA_RTV_DIMENSION_TEXTURE2DMSARRAY:
		desc_out.Texture2DMSArray.ArraySize = rtv_desc.ArraySize;
		desc_out.Texture2DMSArray.FirstArraySlice = rtv_desc.FirstArraySlice;
		break;
	case LRTViewDimention::LUNA_RTV_DIMENSION_TEXTURE3D:
		desc_out.Texture3D.FirstWSlice = rtv_desc.FirstWSlice;
		desc_out.Texture3D.MipSlice = rtv_desc.MipSlice;
		desc_out.Texture3D.WSize = rtv_desc.WSize;
		break;
	default:
		break;
	}
	return desc_out;
}


D3D12_DSV_FLAGS RENDER_API GetBufferDsvFlags(LDSViewFlags dsv_flag)
{
	switch (dsv_flag)
	{
	case luna::render::LUNA_DSV_FLAG_NONE:
		return D3D12_DSV_FLAGS::D3D12_DSV_FLAG_NONE;
		break;
	case luna::render::LUNA_DSV_FLAG_READ_ONLY_DEPTH:
		return D3D12_DSV_FLAGS::D3D12_DSV_FLAG_READ_ONLY_DEPTH;
		break;
	case luna::render::LUNA_DSV_FLAG_READ_ONLY_STENCIL:
		return D3D12_DSV_FLAGS::D3D12_DSV_FLAG_READ_ONLY_STENCIL;
		break;
	default:
		break;
	}
	return D3D12_DSV_FLAGS::D3D12_DSV_FLAG_NONE;
}

D3D12_DSV_DIMENSION RENDER_API GetDsvDimention(RHIViewDimension dsv_dimention)
{
	switch (dsv_dimention)
	{
	case RHIViewDimension::TextureView2D:
		return D3D12_DSV_DIMENSION::D3D12_DSV_DIMENSION_TEXTURE2D;
		break;
	default:
		assert(false);
	}
	return D3D12_DSV_DIMENSION::D3D12_DSV_DIMENSION_UNKNOWN;
}

D3D12_DEPTH_STENCIL_VIEW_DESC RENDER_API GetDepthStencilViewDesc(const LDepthStencilViewDesc& dsv_desc)
{
	D3D12_DEPTH_STENCIL_VIEW_DESC desc_out = {};
	desc_out.Format = GetGraphicFormat(dsv_desc.Format);
	desc_out.Flags = GetBufferDsvFlags(dsv_desc.Flags);
	switch (dsv_desc.ViewDimension)
	{
	case LDSViewDimention::LUNA_DSV_DIMENSION_UNKNOWN:
		break;
	case LDSViewDimention::LUNA_DSV_DIMENSION_TEXTURE1D:
		desc_out.Texture1D.MipSlice = dsv_desc.MipSlice;
		break;
	case LDSViewDimention::LUNA_DSV_DIMENSION_TEXTURE1DARRAY:
		desc_out.Texture1DArray.ArraySize = dsv_desc.ArraySize;
		desc_out.Texture1DArray.FirstArraySlice = dsv_desc.FirstArraySlice;
		desc_out.Texture1DArray.MipSlice = dsv_desc.MipSlice;
		break;
	case LDSViewDimention::LUNA_DSV_DIMENSION_TEXTURE2D:
		desc_out.Texture2D.MipSlice = dsv_desc.MipSlice;
		break;
	case LDSViewDimention::LUNA_DSV_DIMENSION_TEXTURE2DARRAY:
		desc_out.Texture2DArray.ArraySize = dsv_desc.ArraySize;
		desc_out.Texture2DArray.FirstArraySlice = dsv_desc.FirstArraySlice;
		desc_out.Texture2DArray.MipSlice = dsv_desc.MipSlice;
		break;
	case LDSViewDimention::LUNA_DSV_DIMENSION_TEXTURE2DMS:
		desc_out.Texture2DMS.UnusedField_NothingToDefine = dsv_desc.UnusedField_NothingToDefine;
		break;
	case LDSViewDimention::LUNA_DSV_DIMENSION_TEXTURE2DMSARRAY:
		desc_out.Texture2DMSArray.ArraySize = dsv_desc.ArraySize;
		desc_out.Texture2DMSArray.FirstArraySlice = dsv_desc.FirstArraySlice;
		break;
	default:
		break;
	}
	return desc_out;
}

D3D12_RESOURCE_STATES RENDER_API GetResourceState(const LResState& res_state)
{
	switch (res_state)
	{
	case LResState::LUNA_RESOURCE_STATE_COMMON:
		return D3D12_RESOURCE_STATE_COMMON;
	case LResState::LUNA_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER:
		return D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
	case LResState::LUNA_RESOURCE_STATE_INDEX_BUFFER:
		return D3D12_RESOURCE_STATE_INDEX_BUFFER;
	case LResState::LUNA_RESOURCE_STATE_RENDER_TARGET:
		return D3D12_RESOURCE_STATE_RENDER_TARGET;
	case LResState::LUNA_RESOURCE_STATE_UNORDERED_ACCESS:
		return D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
	case LResState::ResStateDepthWrite:
		return D3D12_RESOURCE_STATE_DEPTH_WRITE;
	case LResState::LUNA_RESOURCE_STATE_DEPTH_READ:
		return D3D12_RESOURCE_STATE_DEPTH_READ;
	case LResState::LUNA_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE:
		return D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
	case LResState::LUNA_RESOURCE_STATE_PIXEL_SHADER_RESOURCE:
		return D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	case LResState::LUNA_RESOURCE_STATE_STREAM_OUT:
		return D3D12_RESOURCE_STATE_STREAM_OUT;
	case LResState::LUNA_RESOURCE_STATE_INDIRECT_ARGUMENT:
		return D3D12_RESOURCE_STATE_INDIRECT_ARGUMENT;
	case LResState::LUNA_RESOURCE_STATE_COPY_DEST:
		return D3D12_RESOURCE_STATE_COPY_DEST;
	case LResState::LUNA_RESOURCE_STATE_COPY_SOURCE:
		return D3D12_RESOURCE_STATE_COPY_SOURCE;
	case LResState::LUNA_RESOURCE_STATE_RESOLVE_DEST:
		return D3D12_RESOURCE_STATE_RESOLVE_DEST;
	case LResState::LUNA_RESOURCE_STATE_RESOLVE_SOURCE:
		return D3D12_RESOURCE_STATE_RESOLVE_SOURCE;
	case LResState::LUNA_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE:
		return D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE;
	case LResState::LUNA_RESOURCE_STATE_SHADING_RATE_SOURCE:
		return D3D12_RESOURCE_STATE_SHADING_RATE_SOURCE;
	case LResState::LUNA_RESOURCE_STATE_GENERIC_READ:
		return D3D12_RESOURCE_STATE_GENERIC_READ;
	case LResState::LUNA_RESOURCE_STATE_VIDEO_DECODE_READ:
		return D3D12_RESOURCE_STATE_VIDEO_DECODE_READ;
	case LResState::LUNA_RESOURCE_STATE_VIDEO_DECODE_WRITE:
		return D3D12_RESOURCE_STATE_VIDEO_DECODE_WRITE;
	case LResState::LUNA_RESOURCE_STATE_VIDEO_PROCESS_READ:
		return D3D12_RESOURCE_STATE_VIDEO_PROCESS_READ;
	case LResState::LUNA_RESOURCE_STATE_VIDEO_PROCESS_WRITE:
		return D3D12_RESOURCE_STATE_VIDEO_PROCESS_WRITE;
	case LResState::LUNA_RESOURCE_STATE_VIDEO_ENCODE_READ:
		return D3D12_RESOURCE_STATE_VIDEO_ENCODE_READ;
	case LResState::LUNA_RESOURCE_STATE_VIDEO_ENCODE_WRITE:
		return D3D12_RESOURCE_STATE_VIDEO_ENCODE_WRITE;
	default:
		return D3D12_RESOURCE_STATE_COMMON;
	}
	return D3D12_RESOURCE_STATE_COMMON;
}

RHIShaderSlotType RENDER_API GetBindType(D3D_SHADER_INPUT_TYPE type_in)
{
	switch (type_in)
	{
	case D3D_SIT_CBUFFER:
		return BIND_TYPE_CBUFFER;
	case D3D_SIT_TBUFFER:
		return BIND_TYPE_TBUFFER;
	case D3D_SIT_TEXTURE:
		return BIND_TYPE_TEXTURE;
	case D3D_SIT_SAMPLER:
		return BIND_TYPE_SAMPLER;
	case D3D_SIT_UAV_RWTYPED:
		return BIND_TYPE_UAV_RWTYPED;
	case D3D_SIT_STRUCTURED:
		return BIND_TYPE_STRUCTURED;
	case D3D_SIT_UAV_RWSTRUCTURED:
		return BIND_TYPE_UAV_RWSTRUCTURED;
	case D3D_SIT_BYTEADDRESS:
		return BIND_TYPE_BYTEADDRESS;
	case D3D_SIT_UAV_RWBYTEADDRESS:
		return BIND_TYPE_UAV_RWBYTEADDRESS;
	case D3D_SIT_UAV_APPEND_STRUCTURED:
		return BIND_TYPE_UAV_APPEND_STRUCTURED;
	case D3D_SIT_UAV_CONSUME_STRUCTURED:
		return BIND_TYPE_UAV_CONSUME_STRUCTURED;
	case D3D_SIT_UAV_RWSTRUCTURED_WITH_COUNTER:
		return BIND_TYPE_UAV_RWSTRUCTURED_WITH_COUNTER;
	default:
		break;
	}
	return BIND_TYPE_UNKNOWN;
};

D3D12_PLACED_SUBRESOURCE_FOOTPRINT RENDER_API GetFootPrint(const RHIPlacedSubResFootprint& foot_print)
{
	D3D12_PLACED_SUBRESOURCE_FOOTPRINT out;
	out.Offset = foot_print.Offset;
	out.Footprint.Depth = foot_print.Footprint.Depth;
	out.Footprint.Format = GetGraphicFormat(foot_print.Footprint.Format);
	out.Footprint.Height = foot_print.Footprint.Height;
	out.Footprint.RowPitch = foot_print.Footprint.RowPitch;
	out.Footprint.Width = foot_print.Footprint.Width;
	return out;
}

RHIPlacedSubResFootprint RENDER_API GetFootPrintDx(const D3D12_PLACED_SUBRESOURCE_FOOTPRINT& foot_print)
{
	RHIPlacedSubResFootprint out;
	out.Offset = foot_print.Offset;
	out.Footprint.Depth = foot_print.Footprint.Depth;
	out.Footprint.Format = GetGraphicFormatFromDx(foot_print.Footprint.Format);
	out.Footprint.Height = foot_print.Footprint.Height;
	out.Footprint.RowPitch = foot_print.Footprint.RowPitch;
	out.Footprint.Width = foot_print.Footprint.Width;
	return out;
}
}

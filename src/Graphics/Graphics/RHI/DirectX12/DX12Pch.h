#pragma once
#include "Graphics/RenderConfig.h"
//临时的dx11头文件(纹理压缩)
#include <d3d11.h>
#include <d3d12.h>
#include "d3dx12.h"
#include <DirectXMath.h>
#include <d3dcompiler.h>
#include <DXGI1_6.h>
#include <wrl/client.h>
#include <comdef.h>

#include "Graphics/RHI/RHIDevice.h"
#include "Graphics/RHI/RHIDescriptor.h"

namespace luna::graphics
{

ENABLE_BITMASK_OPERATORS(D3D12_SHADER_VISIBILITY);


D3D12_RESOURCE_STATES RENDER_API DxConvertState(ResourceState state);


using LDirectXGIFactory = IDXGIFactory4;
bool RENDER_API GetDirectXCommondlistType(RHICmdListType type_input, D3D12_COMMAND_LIST_TYPE& type_output);

D3D12_COMPARISON_FUNC RENDER_API GetComparisionFunc(RHIComparisionFunc comparision_func);

D3D12_STATIC_BORDER_COLOR RENDER_API GetBorderColor(const LunarStaticBorderColor& border_color);

D3D12_COMPARISON_FUNC RENDER_API GetCompareFunc(const RHIComparisionFunc& compare_func);

DXGI_FORMAT RENDER_API GetGraphicFormat(const RHITextureFormat& graphic_format);

RHITextureFormat RENDER_API GetGraphicFormatFromDx(const DXGI_FORMAT& graphic_format);

D3D12_RESOURCE_DIMENSION RENDER_API GetResourceDimension(const RHIResDimension& res_dimension);

RHIResDimension RENDER_API GetResourceDimensionDx(const D3D12_RESOURCE_DIMENSION& res_dimension);

D3D12_TEXTURE_LAYOUT RENDER_API GetTextureLayout(const RHITextureLayout& texture_layout);

D3D12_RESOURCE_FLAGS RENDER_API GetGpuResourceFlag(const RHIResourceUsage& gpu_res_flag);

RHIResourceUsage RENDER_API GetGpuResourceFlagDx(const D3D12_RESOURCE_FLAGS& gpu_res_flag);

D3D12_RESOURCE_DESC RENDER_API GetResourceDesc(const RHIResDesc& res_desc);

RHIResDesc RENDER_API GetResourceDescDx(const D3D12_RESOURCE_DESC& res_desc);

D3D12_HEAP_TYPE RENDER_API GetHeapType(const RHIHeapType& heap_type);

D3D12_HEAP_FLAGS RENDER_API GetHeapFlag(const RHIHeapFlag& heap_flag);


D3D12_BUFFER_SRV_FLAGS RENDER_API GetBufferSrvFlags(LBufferSrvFlag srv_flag);

D3D12_SRV_DIMENSION RENDER_API GetSrvDimention(RHIResDimension srv_dimention);

D3D12_SRV_DIMENSION RENDER_API GetSrvDimentionByView(RHIViewDimension view_dimension);

D3D12_UAV_DIMENSION RENDER_API GetUavDimentionByView(RHIViewDimension view_dimension);

D3D12_SHADER_RESOURCE_VIEW_DESC RENDER_API GetShaderResourceViewDesc(const LShaderResourceViewDesc& srv_desc);


D3D12_BUFFER_UAV_FLAGS RENDER_API GetBufferUavFlags(LBufferUavFlag uav_flag);


D3D12_RTV_DIMENSION RENDER_API GetRtvDimention(RHIViewDimension rtv_dimention);

D3D12_RENDER_TARGET_VIEW_DESC RENDER_API GetRenderTargetViewDesc(const LRenderTargetViewDesc& rtv_desc);


D3D12_DSV_FLAGS RENDER_API GetBufferDsvFlags(LDSViewFlags dsv_flag);

D3D12_DSV_DIMENSION RENDER_API GetDsvDimention(RHIViewDimension dsv_dimention);

D3D12_DEPTH_STENCIL_VIEW_DESC RENDER_API GetDepthStencilViewDesc(const LDepthStencilViewDesc& dsv_desc);

D3D12_RESOURCE_STATES RENDER_API GetResourceState(const LResState& res_state);

RHIShaderSlotType RENDER_API GetBindType(D3D_SHADER_INPUT_TYPE type_in);

D3D12_PLACED_SUBRESOURCE_FOOTPRINT RENDER_API GetFootPrint(const RHIPlacedSubResFootprint& foot_print);

RHIPlacedSubResFootprint RENDER_API GetFootPrintDx(const D3D12_PLACED_SUBRESOURCE_FOOTPRINT& foot_print);

D3D12_GRAPHICS_PIPELINE_STATE_DESC GetPipelineDesc(const RHIPipelineStateGraphDrawDesc& pipeline_desc, const RenderPassDesc pass_desc);

D3D12_FILL_MODE GetFillMode(const RHIRasterizerFillMode& pipeline_fill_mode);

D3D12_CULL_MODE GetCullMode(const RHIRasterizerCullMode& pipeline_cull_mode);

D3D12_DEPTH_WRITE_MASK GetDepthMask(bool depth_mask);

D3D12_STENCIL_OP GetStencilOption(const RHIStencilOption& stencil_option);

D3D12_PRIMITIVE_TOPOLOGY_TYPE GetTypologyType(const RHIPrimitiveTopologyType& typology_type);

D3D12_RENDER_TARGET_BLEND_DESC GetRenderTargetBlendDesc(
	const RHIBlendStateTargetDesc& render_blend_desc);

D3D12_BLEND GetRenderBlend(const RHIRenderBlend& render_blend);

D3D12_BLEND_OP GetRenderBlendOption(const RHIBlendOption& render_blend_option);

D3D12_LOGIC_OP GetRenderBlendLogicOption(const RHIBlendLogicOption& render_blend_logic_option);
}

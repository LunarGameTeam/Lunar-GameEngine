#pragma once

#include "render/rhi/rhi_ptr.h"
#include "render/rhi/rhi_types.h"
#include "render/rhi/vertex_layout.h"

namespace luna::render
{

#define VULKAN_ASSERT(res) LUNA_ASSERT((res) == vk::Result::eSuccess)

//vb&ib
struct RHIVertexBufferDesc
{
	RHIResource* mVertexRes = nullptr;
	size_t		 mVertexStride = 0;
	size_t       mOffset = 0;
	size_t       mBufferSize = 0;
};

enum class RHIResUsingState : uint8_t
{
	RESOURCE_USING_STATE_UNIFORM_BUFFER = 0,
	RESOURCE_USING_STATE_VERTEX_BUFFER,
	RESOURCE_USING_STATE_INDEX_BUFFER,
	RESOURCE_USING_STATE_RENDER_TARGET,
	RESOURCE_USING_STATE_DEPTH_WRITE,
	RESOURCE_USING_STATE_DEPTH_READE,
	RESOURCE_USING_STATE_SHADER_RESOURCE_NON_PIXEL,
	RESOURCE_USING_STATE_SHADER_RESOURCE_PIXEL,
	RESOURCE_USING_STATE_SHADER_STREAM_OUT,
	RESOURCE_USING_STATE_SHADER_INDIRECT_ARGUMENT,
	RESOURCE_USING_STATE_SHADER_COPY_DEST,
	RESOURCE_USING_STATE_SHADER_COPY_SOURCE,
	RESOURCE_USING_STATE_SHADER_RESOLVE_DEST,
	RESOURCE_USING_STATE_SHADER_RESOLVE_SOURCE,
	RESOURCE_USING_STATE_RAYTRACING_ACCELERATION_STRUCTURE,
	RESOURCE_USING_STATE_RAYTRACING_SHADING_RATE_SOURCE,
	RESOURCE_USING_STATE_RAYTRACING_GENERIC_READ,
	RESOURCE_USING_STATE_RAYTRACING_SHADING_RATE_PRESENT,
	RESOURCE_USING_STATE_RAYTRACING_SHADING_RATE_PREDICATION
};


enum class RHIPrimitiveTopology
{
	PRIMITIVE_TOPOLOGY_UNDEFINED = 0,
	LineList,
	TriangleList,
	PRIMITIVE_TOPOLOGY_POINTLIST,
	PRIMITIVE_TOPOLOGY_LINESTRIP,
	PRIMITIVE_TOPOLOGY_TRIANGLESTRIP,
	PRIMITIVE_TOPOLOGY_LINELIST_ADJ,
	PRIMITIVE_TOPOLOGY_LINESTRIP_ADJ,
	PRIMITIVE_TOPOLOGY_TRIANGLELIST_ADJ,
	PRIMITIVE_TOPOLOGY_TRIANGLESTRIP_ADJ,
	PRIMITIVE_TOPOLOGY_1_CONTROL_POINT_PATCHLIST,
	PRIMITIVE_TOPOLOGY_2_CONTROL_POINT_PATCHLIST,
	PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST,
	PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST,
	PRIMITIVE_TOPOLOGY_5_CONTROL_POINT_PATCHLIST,
	PRIMITIVE_TOPOLOGY_6_CONTROL_POINT_PATCHLIST,
	PRIMITIVE_TOPOLOGY_7_CONTROL_POINT_PATCHLIST,
	PRIMITIVE_TOPOLOGY_8_CONTROL_POINT_PATCHLIST,
	PRIMITIVE_TOPOLOGY_9_CONTROL_POINT_PATCHLIST,
	PRIMITIVE_TOPOLOGY_10_CONTROL_POINT_PATCHLIST,
	PRIMITIVE_TOPOLOGY_11_CONTROL_POINT_PATCHLIST,
	PRIMITIVE_TOPOLOGY_12_CONTROL_POINT_PATCHLIST,
	PRIMITIVE_TOPOLOGY_13_CONTROL_POINT_PATCHLIST,
	PRIMITIVE_TOPOLOGY_14_CONTROL_POINT_PATCHLIST,
	PRIMITIVE_TOPOLOGY_15_CONTROL_POINT_PATCHLIST,
	PRIMITIVE_TOPOLOGY_16_CONTROL_POINT_PATCHLIST,
	PRIMITIVE_TOPOLOGY_17_CONTROL_POINT_PATCHLIST,
	PRIMITIVE_TOPOLOGY_18_CONTROL_POINT_PATCHLIST,
	PRIMITIVE_TOPOLOGY_19_CONTROL_POINT_PATCHLIST,
	PRIMITIVE_TOPOLOGY_20_CONTROL_POINT_PATCHLIST,
	PRIMITIVE_TOPOLOGY_21_CONTROL_POINT_PATCHLIST,
	PRIMITIVE_TOPOLOGY_22_CONTROL_POINT_PATCHLIST,
	PRIMITIVE_TOPOLOGY_23_CONTROL_POINT_PATCHLIST,
	PRIMITIVE_TOPOLOGY_24_CONTROL_POINT_PATCHLIST,
	PRIMITIVE_TOPOLOGY_25_CONTROL_POINT_PATCHLIST,
	PRIMITIVE_TOPOLOGY_26_CONTROL_POINT_PATCHLIST,
	PRIMITIVE_TOPOLOGY_27_CONTROL_POINT_PATCHLIST,
	PRIMITIVE_TOPOLOGY_28_CONTROL_POINT_PATCHLIST,
	PRIMITIVE_TOPOLOGY_29_CONTROL_POINT_PATCHLIST,
	PRIMITIVE_TOPOLOGY_30_CONTROL_POINT_PATCHLIST,
	PRIMITIVE_TOPOLOGY_31_CONTROL_POINT_PATCHLIST,
	PRIMITIVE_TOPOLOGY_32_CONTROL_POINT_PATCHLIST
};


enum class RHIPrimitiveTopologyType
{
	Undefined = 0,
	Point = 1,
	Line = 2,
	Triangle = 3,
	Patch = 4
};


enum class RHIResType : uint8_t
{
	Undefine = 0,
	Shader,
	RootSignature,
	Pipeline,
	GRAPHIC_RESOURCE_ENGINE_RESOURCE,
	GRAPHIC_RESOURCE_ENGINE_RESOURCE_HEAP,
	GRAPHIC_RESOURCE_ENGINE_DESCRIPTOR,
	GRAPHIC_RESOURCE_ENGINE_DESCRIPTOR_HEAP
};



enum class RHIComparisionFunc : uint8_t
{
	FuncNever = 1,
	FuncLess = 2,
	FuncEqual = 3,
	FuncLessEqual = 4,
	FuncGreater = 5,
	FuncNotEqual = 6,
	FuncGreaterEuqal = 7,
	FuncAlways = 8
};


enum class RHIRenderFilter
{
	FILTER_MIN_MAG_MIP_POINT = 0,
	FILTER_MIN_MAG_POINT_MIP_LINEAR = 0x1,
	FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT = 0x4,
	FILTER_MIN_POINT_MAG_MIP_LINEAR = 0x5,
	FILTER_MIN_LINEAR_MAG_MIP_POINT = 0x10,
	FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR = 0x11,
	FILTER_MIN_MAG_LINEAR_MIP_POINT = 0x14,
	FILTER_MIN_MAG_MIP_LINEAR = 0x15,
	FILTER_ANISOTROPIC = 0x55,
	FILTER_COMPARISON_MIN_MAG_MIP_POINT = 0x80,
	FILTER_COMPARISON_MIN_MAG_POINT_MIP_LINEAR = 0x81,
	FILTER_COMPARISON_MIN_POINT_MAG_LINEAR_MIP_POINT = 0x84,
	FILTER_COMPARISON_MIN_POINT_MAG_MIP_LINEAR = 0x85,
	FILTER_COMPARISON_MIN_LINEAR_MAG_MIP_POINT = 0x90,
	FILTER_COMPARISON_MIN_LINEAR_MAG_POINT_MIP_LINEAR = 0x91,
	FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT = 0x94,
	FILTER_COMPARISON_MIN_MAG_MIP_LINEAR = 0x95,
	FILTER_COMPARISON_ANISOTROPIC = 0xd5,
	FILTER_MINIMUM_MIN_MAG_MIP_POINT = 0x100,
	FILTER_MINIMUM_MIN_MAG_POINT_MIP_LINEAR = 0x101,
	FILTER_MINIMUM_MIN_POINT_MAG_LINEAR_MIP_POINT = 0x104,
	FILTER_MINIMUM_MIN_POINT_MAG_MIP_LINEAR = 0x105,
	FILTER_MINIMUM_MIN_LINEAR_MAG_MIP_POINT = 0x110,
	FILTER_MINIMUM_MIN_LINEAR_MAG_POINT_MIP_LINEAR = 0x111,
	FILTER_MINIMUM_MIN_MAG_LINEAR_MIP_POINT = 0x114,
	FILTER_MINIMUM_MIN_MAG_MIP_LINEAR = 0x115,
	FILTER_MINIMUM_ANISOTROPIC = 0x155,
	FILTER_MAXIMUM_MIN_MAG_MIP_POINT = 0x180,
	FILTER_MAXIMUM_MIN_MAG_POINT_MIP_LINEAR = 0x181,
	FILTER_MAXIMUM_MIN_POINT_MAG_LINEAR_MIP_POINT = 0x184,
	FILTER_MAXIMUM_MIN_POINT_MAG_MIP_LINEAR = 0x185,
	FILTER_MAXIMUM_MIN_LINEAR_MAG_MIP_POINT = 0x190,
	FILTER_MAXIMUM_MIN_LINEAR_MAG_POINT_MIP_LINEAR = 0x191,
	FILTER_MAXIMUM_MIN_MAG_LINEAR_MIP_POINT = 0x194,
	FILTER_MAXIMUM_MIN_MAG_MIP_LINEAR = 0x195,
	FILTER_MAXIMUM_ANISOTROPIC = 0x1d5
};



enum class LunarStaticBorderColor
{
	STATIC_BORDER_COLOR_TRANSPARENT_BLACK = 0,
	STATIC_BORDER_COLOR_OPAQUE_BLACK = (STATIC_BORDER_COLOR_TRANSPARENT_BLACK + 1),
	STATIC_BORDER_COLOR_OPAQUE_WHITE = (STATIC_BORDER_COLOR_OPAQUE_BLACK + 1)
};


enum class RHIHeapType
{
	Default = 1,
	Upload = 2,
	Readback = 3,
	Custom = 4
};


enum class RHIHeapFlag : uint32_t
{
	HEAP_FLAG_NONE = 0,
	HEAP_FLAG_SHARED = 0x1,
	HEAP_FLAG_DENY_BUFFERS = 0x4,
	HEAP_FLAG_ALLOW_DISPLAY = 0x8,
	HEAP_FLAG_SHARED_CROSS_ADAPTER = 0x20,
	HEAP_FLAG_DENY_RT_DS_TEXTURES = 0x40,
	HEAP_FLAG_DENY_NON_RT_DS_TEXTURES = 0x80,
	HEAP_FLAG_HARDWARE_PROTECTED = 0x100,
	HEAP_FLAG_ALLOW_WRITE_WATCH = 0x200,
	HEAP_FLAG_ALLOW_SHADER_ATOMICS = 0x400,
	HEAP_FLAG_CREATE_NOT_RESIDENT = 0x800,
	HEAP_FLAG_CREATE_NOT_ZEROED = 0x1000,
	HEAP_FLAG_ALLOW_ALL_BUFFERS_AND_TEXTURES = 0,
	HEAP_FLAG_ALLOW_ONLY_BUFFERS = 0xc0,
	HEAP_FLAG_ALLOW_ONLY_NON_RT_DS_TEXTURES = 0x44,
	HEAP_FLAG_ALLOW_ONLY_RT_DS_TEXTURES = 0x84
};


enum class RHISrvDimension
{
	SRV_Unknown = 0,
	SRV_Buffer = 1,
	SRV_Texture1D = 2,
	SRV_DIMENSION_TEXTURE1DARRAY = 3,
	SRV_Texture2D = 4,
	SRV_DIMENSION_TEXTURE2DARRAY = 5,
	SRV_DIMENSION_TEXTURE2DMS = 6,
	SRV_DIMENSION_TEXTURE2DMSARRAY = 7,
	SRV_DIMENSION_TEXTURE3D = 8,
	SRV_TextureCube = 9,
	SRV_DIMENSION_TEXTURECUBEARRAY = 10,
	SRV_DIMENSION_RAYTRACING_ACCELERATION_STRUCTURE = 11
};


enum class LBufferSrvFlag
{
	LUNA_BUFFER_SRV_FLAG_NONE = 0,
	LUNA_BUFFER_SRV_FLAG_RAW = 0x1
};


enum class LUAVDimention
{
	LUNA_UAV_DIMENSION_UNKNOWN = 0,
	LUNA_UAV_DIMENSION_BUFFER = 1,
	LUNA_UAV_DIMENSION_TEXTURE1D = 2,
	LUNA_UAV_DIMENSION_TEXTURE1DARRAY = 3,
	LUNA_UAV_DIMENSION_TEXTURE2D = 4,
	LUNA_UAV_DIMENSION_TEXTURE2DARRAY = 5,
	LUNA_UAV_DIMENSION_TEXTURE3D = 8
};


enum class LBufferUavFlag
{
	LUNA_BUFFER_UAV_FLAG_NONE = 0,
	LUNA_BUFFER_UAV_FLAG_RAW = 0x1
};


enum class LRTViewDimention
{
	LUNA_RTV_DIMENSION_UNKNOWN = 0,
	LUNA_RTV_DIMENSION_BUFFER = 1,
	LUNA_RTV_DIMENSION_TEXTURE1D = 2,
	LUNA_RTV_DIMENSION_TEXTURE1DARRAY = 3,
	LUNA_RTV_DIMENSION_TEXTURE2D = 4,
	LUNA_RTV_DIMENSION_TEXTURE2DARRAY = 5,
	LUNA_RTV_DIMENSION_TEXTURE2DMS = 6,
	LUNA_RTV_DIMENSION_TEXTURE2DMSARRAY = 7,
	LUNA_RTV_DIMENSION_TEXTURE3D = 8
};


enum class LDSViewDimention
{
	LUNA_DSV_DIMENSION_UNKNOWN = 0,
	LUNA_DSV_DIMENSION_TEXTURE1D = 1,
	LUNA_DSV_DIMENSION_TEXTURE1DARRAY = 2,
	LUNA_DSV_DIMENSION_TEXTURE2D = 3,
	LUNA_DSV_DIMENSION_TEXTURE2DARRAY = 4,
	LUNA_DSV_DIMENSION_TEXTURE2DMS = 5,
	LUNA_DSV_DIMENSION_TEXTURE2DMSARRAY = 6
};


enum LDSViewFlags
{
	LUNA_DSV_FLAG_NONE = 0,
	LUNA_DSV_FLAG_READ_ONLY_DEPTH = 0x1,
	LUNA_DSV_FLAG_READ_ONLY_STENCIL = 0x2
};

enum ResourceState : uint32_t
{
	kUnknown = 0,
	kCommon = 1 << 0,
	kVertexAndConstantBuffer = 1 << 1,
	kIndexBuffer = 1 << 2,
	kRenderTarget = 1 << 3,
	kUnorderedAccess = 1 << 4,
	kDepthStencilWrite = 1 << 5,
	kDepthStencilRead = 1 << 6,
	kNonPixelShaderResource = 1 << 7,
	kShaderReadOnly = 1 << 8,
	kIndirectArgument = 1 << 9,
	kCopyDest = 1 << 10,
	kCopySource = 1 << 11,
	kRaytracingAccelerationStructure = 1 << 12,
	kShadingRateSource = 1 << 13,
	kPresent = 1 << 14,
	kGenericRead =
	ResourceState::kVertexAndConstantBuffer |
	ResourceState::kIndexBuffer |
	ResourceState::kCopySource |
	ResourceState::kNonPixelShaderResource |
	ResourceState::kShaderReadOnly |
	ResourceState::kIndirectArgument,
	kUndefined = 1 << 15,
};

ENABLE_BITMASK_OPERATORS(ResourceState);

struct ResourceBarrierDesc
{
	RHIResourcePtr mBarrierRes;
	ResourceState  mStateBefore  = ResourceState::kUndefined;
	ResourceState  mStateAfter   = ResourceState::kUndefined;
	uint32_t       mBaseMipLevel = 0;
	uint32_t       mMipLevels    = 1;
	uint32_t       mBaseDepth    = 0;
	uint32_t       mDepth        = 1;
};

enum class LResState : uint32_t
{
	LUNA_RESOURCE_STATE_COMMON = 0,
	LUNA_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER = 0x1,
	LUNA_RESOURCE_STATE_INDEX_BUFFER = 0x2,
	LUNA_RESOURCE_STATE_RENDER_TARGET = 0x4,
	LUNA_RESOURCE_STATE_UNORDERED_ACCESS = 0x8,
	ResStateDepthWrite = 0x10,
	LUNA_RESOURCE_STATE_DEPTH_READ = 0x20,
	LUNA_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE = 0x40,
	LUNA_RESOURCE_STATE_PIXEL_SHADER_RESOURCE = 0x80,
	LUNA_RESOURCE_STATE_STREAM_OUT = 0x100,
	LUNA_RESOURCE_STATE_INDIRECT_ARGUMENT = 0x200,
	LUNA_RESOURCE_STATE_COPY_DEST = 0x400,
	LUNA_RESOURCE_STATE_COPY_SOURCE = 0x800,
	LUNA_RESOURCE_STATE_RESOLVE_DEST = 0x1000,
	LUNA_RESOURCE_STATE_RESOLVE_SOURCE = 0x2000,
	LUNA_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE = 0x400000,
	LUNA_RESOURCE_STATE_SHADING_RATE_SOURCE = 0x1000000,
	LUNA_RESOURCE_STATE_GENERIC_READ = (((((0x1 | 0x2) | 0x40) | 0x80) | 0x200) | 0x800),
	LUNA_RESOURCE_STATE_PRESENT = 0,
	LUNA_RESOURCE_STATE_PREDICATION = 0x200,
	LUNA_RESOURCE_STATE_VIDEO_DECODE_READ = 0x10000,
	LUNA_RESOURCE_STATE_VIDEO_DECODE_WRITE = 0x20000,
	LUNA_RESOURCE_STATE_VIDEO_PROCESS_READ = 0x40000,
	LUNA_RESOURCE_STATE_VIDEO_PROCESS_WRITE = 0x80000,
	LUNA_RESOURCE_STATE_VIDEO_ENCODE_READ = 0x200000,
	LUNA_RESOURCE_STATE_VIDEO_ENCODE_WRITE = 0x800000
};


//颜色掩码
enum LColorWriteEnable
{
	COLOR_WRITE_ENABLE_RED = 1,
	COLOR_WRITE_ENABLE_GREEN = 2,
	COLOR_WRITE_ENABLE_BLUE = 4,
	COLOR_WRITE_ENABLE_ALPHA = 8,
	COLOR_WRITE_ENABLE_ALL = (((COLOR_WRITE_ENABLE_RED | COLOR_WRITE_ENABLE_GREEN) |
	LColorWriteEnable::COLOR_WRITE_ENABLE_BLUE) | LColorWriteEnable::COLOR_WRITE_ENABLE_ALPHA)
};

struct MemoryRequirements
{
	uint64_t size;
	uint64_t alignment;
	uint32_t memory_type_bits;
};

//透明混合格式
enum RHIBlendLogicOption
{
	LOGIC_OP_CLEAR = 0,
	LOGIC_OP_SET = (LOGIC_OP_CLEAR + 1),
	LOGIC_OP_COPY = (LOGIC_OP_SET + 1),
	LOGIC_OP_COPY_INVERTED = (LOGIC_OP_COPY + 1),
	LOGIC_OP_NOOP = (LOGIC_OP_COPY_INVERTED + 1),
	LOGIC_OP_INVERT = (LOGIC_OP_NOOP + 1),
	LOGIC_OP_AND = (LOGIC_OP_INVERT + 1),
	LOGIC_OP_NAND = (LOGIC_OP_AND + 1),
	LOGIC_OP_OR = (LOGIC_OP_NAND + 1),
	LOGIC_OP_NOR = (LOGIC_OP_OR + 1),
	LOGIC_OP_XOR = (LOGIC_OP_NOR + 1),
	LOGIC_OP_EQUIV = (LOGIC_OP_XOR + 1),
	LOGIC_OP_AND_REVERSE = (LOGIC_OP_EQUIV + 1),
	LOGIC_OP_AND_INVERTED = (LOGIC_OP_AND_REVERSE + 1),
	LOGIC_OP_OR_REVERSE = (LOGIC_OP_AND_INVERTED + 1),
	LOGIC_OP_OR_INVERTED = (LOGIC_OP_OR_REVERSE + 1)
};


enum RHIRenderBlend
{
	BLEND_ZERO = 1,
	BLEND_ONE = 2,
	BLEND_SRC_COLOR = 3,
	BLEND_INV_SRC_COLOR = 4,
	BLEND_SRC_ALPHA = 5,
	BLEND_INV_SRC_ALPHA = 6,
	BLEND_DEST_ALPHA = 7,
	BLEND_INV_DEST_ALPHA = 8,
	BLEND_DEST_COLOR = 9,
	BLEND_INV_DEST_COLOR = 10,
	BLEND_SRC_ALPHA_SAT = 11,
	BLEND_BLEND_FACTOR = 14,
	BLEND_INV_BLEND_FACTOR = 15,
	BLEND_SRC1_COLOR = 16,
	BLEND_INV_SRC1_COLOR = 17,
	BLEND_SRC1_ALPHA = 18,
	BLEND_INV_SRC1_ALPHA = 19
};


enum RHIBlendOption
{
	BLEND_OP_ADD = 1,
	BLEND_OP_SUBTRACT = 2,
	BLEND_OP_REV_SUBTRACT = 3,
	BLEND_OP_MIN = 4,
	BLEND_OP_MAX = 5
};


//光栅化格式
enum class RHIRasterizerFillMode
{
	FILL_MODE_WIREFRAME = 2,
	FILL_MODE_SOLID = 3
};


enum RHIRasterizerCullMode
{
	CULL_MODE_NONE = 1,
	CULL_MODE_FRONT = 2,
	CULL_MODE_BACK = 3
};


enum RHIConservativeRasterizerMode
{
	CONSERVATIVE_RASTERIZATION_MODE_OFF = 0,
	CONSERVATIVE_RASTERIZATION_MODE_ON = 1
};


//深度缓冲区格式
enum RHIDepthWriteMask
{
	DEPTH_WRITE_MASK_ZERO = 0,
	DEPTH_WRITE_MASK_ALL = 1
};


enum RHIStencilOption
{
	STENCIL_OP_KEEP = 1,
	STENCIL_OP_ZERO = 2,
	STENCIL_OP_REPLACE = 3,
	STENCIL_OP_INCR_SAT = 4,
	STENCIL_OP_DECR_SAT = 5,
	STENCIL_OP_INVERT = 6,
	STENCIL_OP_INCR = 7,
	STENCIL_OP_DECR = 8
};


enum RHIIndexBufferStripCutValue
{
	INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED = 0,
	INDEX_BUFFER_STRIP_CUT_VALUE_0xFFFF = 1,
	INDEX_BUFFER_STRIP_CUT_VALUE_0xFFFFFFFF = 2
};


enum RHIPipelineStateFlag
{
	PipelineStateNone = 0,
	PipelineStateToolDebug = 0x1
};


enum class RHIDescriptorRangeType
{
	RangeTypeSrv = 0,
	RangeTypeUAV = (RangeTypeSrv + 1),
	RangeTypeCBV = (RangeTypeUAV + 1),
	RangeTypeSampler = (RangeTypeCBV + 1)
};


enum RHIDescriptorRangeFlag
{
	DESCRIPTOR_RANGE_FLAG_NONE = 0,
	DESCRIPTOR_RANGE_FLAG_DESCRIPTORS_VOLATILE = 0x1,
	DESCRIPTOR_RANGE_FLAG_DATA_VOLATILE = 0x2,
	DESCRIPTOR_RANGE_FLAG_DATA_STATIC_WHILE_SET_AT_EXECUTE = 0x4,
	DESCRIPTOR_RANGE_FLAG_DATA_STATIC = 0x8,
	DESCRIPTOR_RANGE_FLAG_DESCRIPTORS_STATIC_KEEPING_BUFFER_BOUNDS_CHECKS = 0x10000
};


enum class RHIShaderVisibility : uint32_t
{
	Visibility_Vertex = 1 << 1,
	Visibility_Hull = 1 << 2,
	Visibility_Domain = 1 << 3,
	Visibility_Geometry = 1 << 4,
	Visibility_Pixel = 1 << 5,
	Visibility_Amplification = 1 << 6,
	Visibility_Mesh = 1 << 7,
	Visibility_All = 0x8fffffff
};

ENABLE_BITMASK_OPERATORS(RHIShaderVisibility);


enum RHIRootSignatureFlag
{
	ROOT_SIGNATURE_FLAG_NONE = 0,
	ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT = 0x1,
	ROOT_SIGNATURE_FLAG_DENY_VERTEX_SHADER_ROOT_ACCESS = 0x2,
	ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS = 0x4,
	ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS = 0x8,
	ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS = 0x10,
	ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS = 0x20,
	ROOT_SIGNATURE_FLAG_ALLOW_STREAM_OUTPUT = 0x40,
	ROOT_SIGNATURE_FLAG_LOCAL_ROOT_SIGNATURE = 0x80,
	ROOT_SIGNATURE_FLAG_DENY_AMPLIFICATION_SHADER_ROOT_ACCESS = 0x100,
	ROOT_SIGNATURE_FLAG_DENY_MESH_SHADER_ROOT_ACCESS = 0x200
};


enum class DescriptorHeapType
{
	CBV_SRV_UAV = 0,
	SAMPLER = (CBV_SRV_UAV + 1),
	RTV = (SAMPLER + 1),
	DSV = (RTV + 1),
	NUM_TYPES = (DSV + 1)
};

enum class DescriptorPoolUseType
{
	AllocateStaticDescriptor = 0,
	AllocateDynamicDescriptor = (AllocateStaticDescriptor + 1)
};

enum DescriptorHeapFlag
{
	DESCRIPTOR_HEAP_FLAG_NONE = 0,
	DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE = 0x1
};

enum LunaShaderMappingFunc
{
};

enum class RHITextureAddressMode
{
	TEXTURE_ADDRESS_MODE_WRAP = 1,
	TEXTURE_ADDRESS_MODE_MIRROR = 2,
	TEXTURE_ADDRESS_MODE_CLAMP = 3,
	TEXTURE_ADDRESS_MODE_BORDER = 4,
	TEXTURE_ADDRESS_MODE_MIRROR_ONCE = 5
};

enum class RHITextureFormat
{
	FORMAT_UNKNOWN = 0,
	FORMAT_R8G8B8A8_TYPELESS,
	FORMAT_R8G8BB8A8_UNORM,
	FORMAT_R8G8B8A8_UNORM_SRGB,
	FORMAT_R8G8B8A8_UINT,
	FORMAT_R8G8B8A8_SNORM,
	FORMAT_R8G8B8A8_SINT,

	FORMAT_B8G8R8A8_TYPELESS,
	FORMAT_B8G8R8A8_UNORM_SRGB,
	FORMAT_B8G8R8X8_TYPELESS,
	FORMAT_B8G8R8X8_UNORM_SRGB,

	FORMAT_R32G32B32A32_TYPELESS,
	FORMAT_R32G32B32A32_FLOAT,
	FORMAT_R32G32B32A32_UINT,
	FORMAT_R32G32B32A32_SINT,
	FORMAT_R32G32B32_TYPELESS,
	FORMAT_R32G32B32_FLOAT,
	FORMAT_R32G32B32_UINT,
	FORMAT_R32G32B32_SINT,

	FORMAT_R16G16B16A16_TYPELESS,
	FORMAT_R16G16B16A16_FLOAT,
	FORMAT_R16G16B16A16_UNORM,
	FORMAT_R16G16B16A16_UINT,
	FORMAT_R16G16B16A16_SNORM,
	FORMAT_R16G16B16A16_SINT,

	FORMAT_R32G32_TYPELESS,
	FORMAT_R32G32_FLOAT,
	FORMAT_R32G32_UINT,
	FORMAT_R32G32_SINT,

	FORMAT_R32G8X24_TYPELESS,

	FORMAT_D32_FLOAT_S8X24_UINT,
	FORMAT_R32_FLOAT_X8X24_TYPELESS,
	FORMAT_X32_TYPELESS_G8X24_UINT,

	FORMAT_R10G10B10A2_TYPELESS,
	FORMAT_R10G10B10A2_UNORM,
	FORMAT_R10G10B10A2_UINT,
	FORMAT_R11G11B10_FLOAT,
	FORMAT_R16G16_TYPELESS ,
	FORMAT_R16G16_FLOAT ,
	FORMAT_R16G16_UNORM ,
	FORMAT_R16G16_UINT ,
	FORMAT_R16G16_SNORM ,
	FORMAT_R16G16_SINT,
	FORMAT_R32_TYPELESS,
	FORMAT_D32_FLOAT,
	FORMAT_R32_FLOAT,
	FORMAT_R32_UINT,
	FORMAT_R32_SINT,
	FORMAT_R24G8_TYPELESS,
	FORMAT_D24_UNORM_S8_UINT,
	FORMAT_R24_UNORM_X8_TYPELESS,
	FORMAT_X24_TYPELESS_G8_UINT,

	FORMAT_R8G8_TYPELESS,
	FORMAT_R8G8_UNORM,
	FORMAT_R8G8_UINT,
	FORMAT_R8G8_SNORM,
	FORMAT_R8G8_SINT,

	FORMAT_R16_TYPELESS,
	FORMAT_R16_FLOAT,
	FORMAT_D16_UNORM,
	FORMAT_R16_UNORM,
	FORMAT_R16_UINT,
	FORMAT_R16_SNORM,
	FORMAT_R16_SINT,
	FORMAT_R8_TYPELESS,
	FORMAT_R8_UNORM,
	FORMAT_R8_UINT,
	FORMAT_R8_SNORM,
	FORMAT_R8_SINT,
	FORMAT_A8_UNORM,
	FORMAT_R1_UNORM,

	FORMAT_R9G9B9E5_SHAREDEXP,
	FORMAT_R8G8_B8G8_UNORM,
	FORMAT_G8R8_G8B8_UNORM,
	FORMAT_BC1_TYPELESS,
	FORMAT_BC1_UNORM,
	FORMAT_BC1_UNORM_SRGB,
	FORMAT_BC2_TYPELESS,
	FORMAT_BC2_UNORM,
	FORMAT_BC2_UNORM_SRGB,
	FORMAT_BC3_TYPELESS,
	FORMAT_BC3_UNORM,
	FORMAT_BC3_UNORM_SRGB,
	FORMAT_BC4_TYPELESS,
	FORMAT_BC4_UNORM,
	FORMAT_BC4_SNORM,
	FORMAT_BC5_TYPELESS,
	FORMAT_BC5_UNORM,
	FORMAT_BC5_SNORM,
	FORMAT_B5G6R5_UNORM,
	FORMAT_B5G5R5A1_UNORM,
	FORMAT_B8G8R8A8_UNORM,
	FORMAT_B8G8R8X8_UNORM,
	FORMAT_R10G10B10_XR_BIAS_A2_UNORM,

	FORMAT_BC6H_TYPELESS,
	FORMAT_BC6H_UF16,
	FORMAT_BC6H_SF16,
	FORMAT_BC7_TYPELESS,
	FORMAT_BC7_UNORM,
	FORMAT_BC7_UNORM_SRGB,
	FORMAT_AYUV,
	FORMAT_Y410,
	FORMAT_Y416,
	FORMAT_NV12,
	FORMAT_P010,
	FORMAT_P016,
	FORMAT_420_OPAQUE,
	FORMAT_YUY2,
	FORMAT_Y210,
	FORMAT_Y216,
	FORMAT_NV11,
	FORMAT_AI44,
	FORMAT_IA44,
	FORMAT_P8,
	FORMAT_A8P8,
	FORMAT_B4G4R4A4_UNORM,
	FORMAT_P208,
	FORMAT_V208,
	FORMAT_V408,
	FORMAT_SAMPLER_FEEDBACK_MIN_MIP_OPAQUE,
	FORMAT_SAMPLER_FEEDBACK_MIP_REGION_USED_OPAQUE,
	FORMAT_FORCE_UINT = 0x7fffffff
};

enum class RHIResDimension
{
	Unknown = 0,
	Buffer = 1,
	Texture1D = 2,
	Texture2D = 3,
	Texture3D = 4
};

enum class RHIViewDimension
{
	BufferView,
	TextureView1D,
	TextureView1DArray,
	TextureView2D,
	TextureView2DArray,
	TextureView3D,
	TextureView3DArray,
	TextureViewCube,
	TextureViewCubeArray,
	Unknown
};

enum class RHITextureLayout
{
	LayoutUnknown = 0,
	TEXTURE_LAYOUT_ROW_MAJOR = 1,
	TEXTURE_LAYOUT_64KB_UNDEFINED_SWIZZLE = 2,
	TEXTURE_LAYOUT_64KB_STANDARD_SWIZZLE = 3
};

enum class RHIBufferUsage
{
	TransferSrcBit = 0x00000001,
	TransferDstBit = 0x00000002,
	VK_BUFFER_USAGE_UNIFORM_TEXEL_BUFFER_BIT = 0x00000004,
	VK_BUFFER_USAGE_STORAGE_TEXEL_BUFFER_BIT = 0x00000008,
	UniformBufferBit = 0x00000010,
	VK_BUFFER_USAGE_STORAGE_BUFFER_BIT = 0x00000020,
	IndexBufferBit = 0x00000040,
	VertexBufferBit = 0x00000080,
};
ENABLE_BITMASK_OPERATORS(RHIBufferUsage);

enum class RHIImageUsage
{
	TransferSrcBit = 0x00000001,
	TransferDstBit = 0x00000002,
	SampledBit = 0x00000004,
	StorageBit = 0x00000008,
	ColorAttachmentBit = 0x00000010,
	DepthStencilBit = 0x00000020,
	VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT = 0x00000040,
	VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT = 0x00000080,
	VK_IMAGE_USAGE_SHADING_RATE_IMAGE_BIT_NV = 0x00000100,
	VK_IMAGE_USAGE_FRAGMENT_DENSITY_MAP_BIT_EXT = 0x00000200,
	VK_IMAGE_USAGE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR = VK_IMAGE_USAGE_SHADING_RATE_IMAGE_BIT_NV,
	VK_IMAGE_USAGE_FLAG_BITS_MAX_ENUM = 0x7FFFFFFF
};
ENABLE_BITMASK_OPERATORS(RHIImageUsage);


enum RHIResourceUsage 
{
	RESOURCE_FLAG_NONE = 0,
	RESOURCE_FLAG_ALLOW_RENDER_TARGET = 0x1,
	RESOURCE_FLAG_ALLOW_DEPTH_STENCIL = 0x2,
	RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS = 0x4,
	RESOURCE_FLAG_DENY_SHADER_RESOURCE = 0x8,
	RESOURCE_FLAG_ALLOW_CROSS_ADAPTER = 0x10,
	RESOURCE_FLAG_ALLOW_SIMULTANEOUS_ACCESS = 0x20,
	RESOURCE_FLAG_VIDEO_DECODE_REFERENCE_ONLY = 0x40
};



struct RHISubResFootprint
{
	RHITextureFormat Format;
	uint32_t Width;
	uint32_t Height;
	uint32_t Depth;
	uint32_t RowPitch;
};


struct RHIPlacedSubResFootprint
{
	UINT64 Offset;
	RHISubResFootprint Footprint;
};



enum RHIShaderSlotType
{
	BIND_TYPE_CBUFFER = 0,
	BIND_TYPE_TBUFFER = (BIND_TYPE_CBUFFER + 1),
	BIND_TYPE_TEXTURE = (BIND_TYPE_TBUFFER + 1),
	BIND_TYPE_SAMPLER = (BIND_TYPE_TEXTURE + 1),
	BIND_TYPE_UAV_RWTYPED = (BIND_TYPE_SAMPLER + 1),
	BIND_TYPE_STRUCTURED = (BIND_TYPE_UAV_RWTYPED + 1),
	BIND_TYPE_UAV_RWSTRUCTURED = (BIND_TYPE_STRUCTURED + 1),
	BIND_TYPE_BYTEADDRESS = (BIND_TYPE_UAV_RWSTRUCTURED + 1),
	BIND_TYPE_UAV_RWBYTEADDRESS = (BIND_TYPE_BYTEADDRESS + 1),
	BIND_TYPE_UAV_APPEND_STRUCTURED = (BIND_TYPE_UAV_RWBYTEADDRESS + 1),
	BIND_TYPE_UAV_CONSUME_STRUCTURED = (BIND_TYPE_UAV_APPEND_STRUCTURED + 1),
	BIND_TYPE_UAV_RWSTRUCTURED_WITH_COUNTER = (BIND_TYPE_UAV_CONSUME_STRUCTURED + 1),
	BIND_TYPE_UNKNOWN = (BIND_TYPE_UAV_RWSTRUCTURED_WITH_COUNTER + 1),
};


enum class RHIShaderType
{
	Vertex = 0,
	Pixel,
	Geomtry,
	Compute,
	Hull,
	Domin,
	Unknown,
};

enum class RHICmdListType
{
	Graphic3D = 0,
	Compute,
	Copy,
	Raytracing
};

enum class RHICmdListState
{
	State_Null = 0,
	State_Preparing,
	State_Dealing,
	State_Finished
};

struct RHIGraphicSampleCount
{
	int32_t Count = 1;
	int32_t Quality = 0;
};


enum class RHIViewType
{
	Uniform_Invalid = 0,

	kConstantBuffer,
	kTexture,
	kSampler,
	kUnorderedAccess,
	kRenderTarget,
	kDepthStencil,
	kRWTexture,
	kBuffer,
	kRWBuffer,
	kStructuredBuffer,
	kRWStructuredBuffer,
	kAccelerationStructure,
	kShadingRateSource,
};


//交换链，用于绑定渲染窗口
struct RHISwapchainDesc
{
	uint32_t  mWidth;
	uint32_t  mHeight;
	int32_t mFrameNumber;
};

inline void GetFormatInfo(
	size_t width,
	size_t height,
	RHITextureFormat format,
	size_t& totalBytes,
	size_t& rowBytes,
	size_t& rows,
	uint32_t alignment)
{
	uint8_t pixelSize = 0;
	switch (format)
	{
	case RHITextureFormat::FORMAT_R8G8BB8A8_UNORM:
		pixelSize = 4;
		break;
	case RHITextureFormat::FORMAT_D24_UNORM_S8_UINT:
		pixelSize = 4;
		break;
	default:
		assert(false);
		break;
	}
	rowBytes = width * pixelSize;
	rowBytes = Alignment(rowBytes, alignment);
	rows = height;
	totalBytes = rowBytes * rows;
};
}

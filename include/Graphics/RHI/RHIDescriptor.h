#pragma once
#include "Graphics/RHI/RHIPch.h"
#include "Graphics/RHI/RHITypes.h"
#include "Graphics/RHI/RHIResource.h"
#include "Graphics/RHI/RHIDescriptorHeap.h"
#include "Graphics/RHI/RHIPtr.h"

namespace luna::graphics
{

struct LShaderResourceViewDesc
{
	RHITextureFormat Format                      = RHITextureFormat::FORMAT_UNKNOWN;
	RHISrvDimension  ViewDimension               = RHISrvDimension::SRV_Unknown;
	uint32_t         Shader4ComponentMapping     = 0;
	//buffer
	uint64_t         FirstElement                = 0;
	uint32_t         NumElements                 = 0;
	uint32_t         StructureByteStride         = 0;
	LBufferSrvFlag   Flags                       = LBufferSrvFlag::LUNA_BUFFER_SRV_FLAG_NONE;
	//texture1D
	uint32_t         MostDetailedMip             = 0;
	uint32_t         MipLevels                   = 0;
	uint32_t         ResourceMinLODClamp         = 0;
	//texture1Darray
	uint32_t         FirstArraySlice             = 0;
	uint32_t         ArraySize                   = 0;
	//texture2D
	uint32_t         PlaneSlice                  = 0;
	//texture2DMS
	uint32_t         UnusedField_NothingToDefine = 0;
	//textureCubeArray
	uint32_t         First2DArrayFace            = 0;
	uint32_t         NumCubes                    = 0;
	//raytracing
	uint64_t         Location                    = 0;
};


struct LUnorderedAccessViewDesc
{
	RHITextureFormat Format               = RHITextureFormat::FORMAT_UNKNOWN;
	LUAVDimention    ViewDimension        = LUAVDimention::LUNA_UAV_DIMENSION_UNKNOWN;
	//buffer
	uint64_t         FirstElement         = 0;
	uint32_t         NumElements          = 0;
	uint32_t         StructureByteStride  = 0;
	uint64_t         CounterOffsetInBytes = 0;
	LBufferUavFlag   Flags                = LBufferUavFlag::LUNA_BUFFER_UAV_FLAG_NONE;
	//texture1D
	uint32_t         MipSlice             = 0;
	//texture1D array
	uint32_t         FirstArraySlice      = 0;
	uint32_t         ArraySize            = 0;
	//texture2D
	uint32_t         PlaneSlice           = 0;
	//texture3D
	uint32_t         FirstWSlice          = 0;
	uint32_t         WSize                = 0;
};


struct LRenderTargetViewDesc
{
	RHITextureFormat Format = RHITextureFormat::FORMAT_UNKNOWN;
	LRTViewDimention ViewDimension = LRTViewDimention::LUNA_RTV_DIMENSION_UNKNOWN;
	//buffer
	uint64_t FirstElement = 0;
	uint32_t NumElements = 0;
	//texture1D
	uint32_t MipSlice = 0;
	//texture1D array
	uint32_t FirstArraySlice = 0;
	uint32_t ArraySize = 0;
	//texture2D
	uint32_t PlaneSlice = 0;
	//texture2D ms
	uint32_t UnusedField_NothingToDefine = 0;
	//texture3D
	uint32_t FirstWSlice = 0;
	uint32_t WSize = 0;
};


struct LDepthStencilViewDesc
{
	RHITextureFormat Format                      = RHITextureFormat::FORMAT_UNKNOWN;
	LDSViewDimention ViewDimension               = LDSViewDimention::LUNA_DSV_DIMENSION_UNKNOWN;
	LDSViewFlags     Flags                       = LDSViewFlags::LUNA_DSV_FLAG_NONE;
	//texture1D
	uint32_t         MipSlice                    = 0;
	//texture1D array
	uint32_t         FirstArraySlice             = 0;
	uint32_t         ArraySize                   = 0;
	//texture2D ms
	uint32_t         UnusedField_NothingToDefine = 0;
};



struct ViewDesc
{
	RHIViewType      mViewType;
	RHIViewDimension mViewDimension;
	uint32_t         mBaseMipLevel    = 0;
	uint32_t         mLevelCount      = 1;
	uint32_t         mBaseArrayLayer  = 0;
	uint32_t         mLayerCount      = 1;
	uint32_t         mPlaneSlice      = 0;
	uint64_t         mOffset          = 0;
	uint32_t         mStructureStride = 0;
	uint64_t         mBufferSize      = static_cast<uint64_t>(-1);

};
//描述符的创建绑定格式
struct RHIDescriptorDesc
{
	LShaderResourceViewDesc  mSrvDesc;
	LUnorderedAccessViewDesc mUavDesc;
	LRenderTargetViewDesc    mRtvDesc;
	LDepthStencilViewDesc    mDsvDesc;

	RHIViewType				 mDescriptorType = RHIViewType::Uniform_Invalid;
	size_t                   mOffset = 0;
	size_t                   mBlockSize = 0;
	
};

//描述符，用于绑定资源
class RENDER_API RHIView : public RHIObject
{	
public:
	ViewDesc mViewDesc;
	RHIResourcePtr mBindResource;
	RHIViewType mViewType;

	RHIView(const ViewDesc& desc):
		mViewDesc(desc),
		mViewType(desc.mViewType)
	{

	};

	virtual ~RHIView()
	{
	};
	
	virtual void BindResource(RHIResource* buffer_data) {};

};
}


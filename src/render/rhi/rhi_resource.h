#pragma once
#include "render/pch.h"
#include "render/rhi/rhi_types.h"
#include "render/rhi/rhi_pch.h"
#include "render/rhi/rhi_ptr.h"
#include "render/rhi/rhi_memory.h"


namespace luna::render
{

enum class ResourceType
{
	kUnknown,
	kBuffer,
	kTexture,
	kSampler,
	kAccelerationStructure,
};


struct RHIGpuResourceDesc
{
	ResourceType          mType            = ResourceType::kUnknown;
	RHIResDimension       Dimension        = RHIResDimension::Texture2D;
	uint32_t              Alignment        = 0;
	uint32_t              Width            = 0;
	uint32_t              Height           = 0;
	uint16_t              DepthOrArraySize = 1;
	uint16_t              MipLevels        = 1;
	RHITextureFormat      Format           = RHITextureFormat::FORMAT_UNKNOWN;
	RHIGraphicSampleCount SampleDesc;
	RHITextureLayout      Layout           = RHITextureLayout::LayoutUnknown;
	RHIBufferUsage        mBufferUsage;
	RHIResourceUsage      mUsage; 	
	RHIImageUsage         mImageUsage;
};

enum class TextureMemoryType
{
	DDS = 0,
	WIC
};


enum class TextureCompressType
{
	TEXTURE_COMPRESS_NONE = 0,
	TEXTURE_COMPRESS_BC1,
	TEXTURE_COMPRESS_BC2,
	TEXTURE_COMPRESS_BC3,
	TEXTURE_COMPRESS_BC4,
	TEXTURE_COMPRESS_BC5,
	TEXTURE_COMPRESS_BC6,
	TEXTURE_COMPRESS_BC7
};


struct RHITextureDesc
{
	RHIHeapFlag heap_flag_in;
	bool        if_gen_mipmap; //是否为无mipmap的纹理创建mipmap
	bool        if_force_srgb; //是否强制转换为线性空间纹理
	int         max_size; //纹理最大大小
};

struct RHIBufferDesc
{
	size_t mSize = 0;
	RHIBufferUsage mBufferUsage;
};

struct RHIResDesc
{
	RHIHeapType ResHeapType;
	RHIGpuResourceDesc Desc = {};
};

class RENDER_API RHIResource : public RHIObject
{	
public:
	RHIResDesc mResDesc;

	uint32_t         mWidth     = 0;
	uint32_t         mHeight    = 0;
	uint32_t         mDepth     = 1;
	uint16_t         mMipLevels = 1;
	RHIResDimension  mDimension = RHIResDimension::Unknown;
	ResourceType     mResType   = ResourceType::kUnknown;
	RHITextureFormat mFormat;

	RHIBufferUsage   mBufferUsage;
	RHIImageUsage    mImageUsage;

	RHIMemoryPtr     mBindMemory;
	size_t           mResSize        = 0;
	ResourceState    m_initial_state = ResourceState::kUnknown;


	RHIResource() = default;
	RHIResource(const RHIResDesc& desc) :
		mWidth(desc.Desc.Width),
		mHeight(desc.Desc.Height),
		mDepth(desc.Desc.DepthOrArraySize),
		mDimension(desc.Desc.Dimension),
		mResType(desc.Desc.mType),
		mFormat(desc.Desc.Format),
		mImageUsage(desc.Desc.mImageUsage),
		mBufferUsage(desc.Desc.mBufferUsage)
	{

	}

	~RHIResource() override
	{

	};

	virtual void BindMemory(RHIMemory* memory, uint64_t offset) {};

	void SetInitialState(ResourceState state) 
	{
		m_initial_state = state;
	};

	ResourceState GetInitialState() const 
	{
		return m_initial_state;
	};

	virtual void UpdateUploadBuffer(size_t offset, const void* src, size_t size) = 0;		

	virtual  void* Map() = 0;
	

	virtual void Unmap() = 0;
	

	const RHIResDesc& GetDesc() const
	{
		return mResDesc;
	}

	const MemoryRequirements& GetMemoryRequirements() const
	{
		return mMemoryLayout;
	}

protected:
	MemoryRequirements mMemoryLayout;

};

class RHIResource;



}

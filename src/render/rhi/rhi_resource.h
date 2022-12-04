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

	ResourceType          mType = ResourceType::kUnknown;
	RHIResDimension       Dimension = RHIResDimension::Texture2D;
	uint32_t              Alignment = 0;
	uint32_t              Width = 0;
	uint32_t              Height = 0;
	uint16_t              DepthOrArraySize = 1;
	uint16_t              MipLevels = 1;
	RHITextureFormat      Format = RHITextureFormat::FORMAT_UNKNOWN;
	RHIGraphicSampleCount SampleDesc;
	RHITextureLayout      Layout = RHITextureLayout::LayoutUnknown;
	RHIBufferUsage        mBufferUsage;
	RHIResourceUsage      mUsage;
	RHIImageUsage         mImageUsage;
};

class RENDER_API RHIResource : public RHIObject
{	
public:
	RHIResDesc mResDesc;
	RHIMemoryPtr     mBindMemory;
	ResourceState    m_initial_state = ResourceState::kUnknown;


	RHIResource() = default;
	RHIResource(const RHIResDesc& desc) :
		mResDesc(desc)
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

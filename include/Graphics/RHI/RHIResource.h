#pragma once
#include "Graphics/RenderConfig.h"
#include "Graphics/RHI/RHITypes.h"
#include "Graphics/RHI/RHIPch.h"
#include "Graphics/RHI/RHIPtr.h"
#include "Graphics/RHI/RHIMemory.h"


namespace luna::graphics
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
	bool        if_gen_mipmap = false; //是否为无mipmap的纹理创建mipmap
	bool        if_force_srgb = false; //是否强制转换为线性空间纹理
	int         max_size = 0; //纹理最大大小
};

struct RHIBufferDesc
{
	size_t mSize = 0;
	RHIBufferUsage mBufferUsage;
};

struct RHIResDesc
{
	RHIHeapType ResHeapType;
	bool                  mUseVma = true;
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
	RHIResource();

	RHIResource(const RHIResDesc& desc);

	RHIResource(const RHIBufferDesc& desc);

	~RHIResource() override;

	void ResetResourceBufferSize(size_t newSize);

	virtual void BindMemory(RHIMemory* memory, uint64_t offset) = 0;

	virtual void BindMemory(RHIHeapType type) = 0;

	void SetInitialState(ResourceState state);

	ResourceState GetInitialState() const;

	virtual void UpdateUploadBuffer(size_t offset, const void* src, size_t size) = 0;		

	virtual  void* Map() = 0;

	virtual void Unmap() = 0;

	const RHIResDesc& GetDesc() const;

	const MemoryRequirements& GetMemoryRequirements() const;
	
	ResourceState    mState = ResourceState::kUndefined;
protected:
	RHIResDesc mResDesc;

	RHIMemoryPtr     mBindMemory;

	mutable bool mSizeDirty = true;

	mutable MemoryRequirements mMemoryLayout;

	virtual void ResetResourceBufferSizeDeviceData(size_t newSize) = 0;

	virtual void RefreshMemoryRequirements() const = 0;
};

class RHIResource;



}

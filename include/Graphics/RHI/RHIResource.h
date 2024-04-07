#pragma once
#include "Graphics/RenderConfig.h"
#include "Graphics/RHI/RHITypes.h"
#include "Graphics/RHI/RHIPch.h"
#include "Graphics/RHI/RHIPtr.h"
#include "Graphics/RHI/RHIMemory.h"
#include "Core/Reflection/Reflection.h"
#include "Core/Scripting/BindingTraits.h"
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

struct RHISubResourceSizeDesc
{
	size_t mOffset = 0;
	size_t mSize = 0;
	size_t mWidth = 0;
	size_t mHeight = 0;
	size_t mRowPitch = 0;
};

struct RHISubResourceCopyLayerDesc
{
	LArray<RHISubResourceSizeDesc> mEachMipmapLevelSize;
};

struct RHISubResourceCopyDesc
{
	LArray<RHISubResourceCopyLayerDesc> mEachArrayMember;
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

	const RHISubResourceCopyDesc& GetCopyDesc() const;
	
	ResourceState    mState = ResourceState::kUndefined;
protected:
	RHIResDesc mResDesc;

	RHIMemoryPtr     mBindMemory;

	mutable bool mSizeDirty = true;

	mutable MemoryRequirements mMemoryLayout;

	mutable RHISubResourceCopyDesc mMemoryCopyDesc;

	virtual void ResetResourceBufferSizeDeviceData(size_t newSize) = 0;

	virtual void RefreshMemoryRequirements() const = 0;
};

}
namespace luna
{
	RegisterType(RENDER_API, graphics::RHIResource, RHIResource);

}
namespace luna::binding
{
	template<> struct binding_converter<graphics::RHIResource*> : native_converter<graphics::RHIResource> { };
	template<> struct binding_proxy<graphics::RHIResource> : native_binding_proxy<graphics::RHIResource> { };
}

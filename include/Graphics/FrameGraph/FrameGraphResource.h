#pragma once

#include "Core/Foundation/String.h"
#include "Core/Foundation/Container.h"
#include "Core/Reflection/Reflection.h"

#include "Graphics/RHI/RHIPch.h"
#include "Graphics/RHI/RHIResource.h"
#include "Graphics/RHI/RHIResource.h"
#include "Graphics/RHI/RHIDescriptor.h"

namespace luna::graphics
{

class FrameGraphBuilder;

class FGResource
{
public:
	size_t         mUniqueId;

	LString        mName;

	RHIResDesc     mDesc;

	RHIResource*   mExternalRes;

	FrameGraphBuilder* mBuilder;

	FGResource() = delete;

	FGResource(size_t uniqueId,const LString& name, RHIResDesc desc, FrameGraphBuilder* builder) :
		mUniqueId(uniqueId),
		mName(name),
		mDesc(desc),
		mBuilder(builder),
		mExternalRes(nullptr)
	{

	}

	~FGResource();

	const LString& GetName() const
	{
		return mName;
	}

	inline ResourceType GetResourceType() const
	{
		return mDesc.mType;
	}

	inline const RHIResDesc& GetDesc() const
	{
		return mDesc;
	}

	RHIResource* GetExternalResource()
	{
		return mExternalRes;
	}

	bool CheckIsExternal()
	{
		return mExternalRes != nullptr;
	}

	void BindExternalResource(RHIResource* val)
	{
		mExternalRes = val;
	}
private:
};

RHIResDesc GenerateTexture2DRhiDesc(
	uint32_t width,
	uint32_t height,
	RHITextureFormat format,
	RHIImageUsage usage
);

class FGTexture : public FGResource
{
public:
	//2D Texture
	FGTexture(
		size_t uniqueId,
		const LString& name,
		uint32_t width,
		uint32_t height,
		RHITextureFormat format,
		RHIImageUsage usage,
		FrameGraphBuilder* builder
	):
		FGResource(uniqueId, name, GenerateTexture2DRhiDesc(width, height, format, usage), builder)
	{

	}
	

};

struct FGResourceView
{	
	FGResource* mVirtualRes = nullptr;

	ViewDesc mRHIViewDesc;
	RHIViewPtr mRHIView = nullptr;
};

enum AdaptoHeaprLevel
{
	HEAP_LEVEL_FLAG_1,
	HEAP_LEVEL_FLAG_2
};

}

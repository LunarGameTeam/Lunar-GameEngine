#pragma once

#include "Core/Foundation/String.h"
#include "Core/Foundation/Container.h"
#include "Core/Reflection/Reflection.h"

#include "Graphics/RHI/RHIPch.h"
#include "Graphics/RHI/RHIResource.h"
#include "Graphics/RHI/RHIResource.h"
#include "Graphics/RHI/RHIDescriptor.h"

namespace luna::render
{


struct FGResource
{
public:

	LString        mName;
	RHIResDesc     mDesc;
	bool mExternal = false;
	RHIResourcePtr mRes;

	FGResource() = delete;
	FGResource(const LString& name, RHIResDesc desc) :
		mName(name),
		mDesc(desc),
		mExternal(false)
	{

	}

	FGResource(const LString& name, RHIResource* res) :
		mName(name),
		mRes(res),
		mExternal(true)
	{

	}

	~FGResource() = default;

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

	RHIResourcePtr GetRHIResource()
	{
		return mRes;
	}

	void SetRHIResource(RHIResource* val)
	{
		mRes = val;
	}
};

struct FGTexture : FGResource
{
	FGTexture(const LString& name, RHIResDesc desc):
		FGResource(name, desc)
	{

	}

	FGTexture(const LString& name, RHIResource* res):
		FGResource(name, res)
	{

	}
	RHITextureDesc mTextureDesc;

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

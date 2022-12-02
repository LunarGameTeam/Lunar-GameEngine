#pragma once

#include "render/rhi/rhi_pch.h"
#include "render/rhi/rhi_resource.h"
#include "render/rhi/rhi_resource.h"
#include "render/rhi/rhi_descriptor.h"

namespace luna::render
{
const static size_t max_frame_graph_res_size = 1024 * 1024 * 512;
const static size_t max_frame_graph_uniform_buffer_size = 65536 * 128 * 16;
const static size_t max_uniform_buffer_size = 65536 * 64;

enum class VirtualResType
{
	Unknown = 0,
	Buffer = 1,
	Texture = 2
};

class FGVirtualRes
{
protected:
	LString mName;
	RHIResDesc mDesc;
	VirtualResType mType = VirtualResType::Unknown;
	RHIResourcePtr mBuffer;
public:
	FGVirtualRes(const LString& name, const RHIResDesc& desc, VirtualResType type);

	size_t GetResourceSize();

	const LString& GetName() const
	{
		return mName;
	}

	const RHIResDesc& GetDesc() const
	{
		return mDesc;
	}

	RHIResourcePtr GetRHIResource()
	{
		return mBuffer;
	}
	void SetRHIResource(RHIResource* rhi_texture)
	{
		mBuffer = rhi_texture;
	}

	virtual ~FGVirtualRes();
};


class FGVirtualBuffer : public FGVirtualRes
{
public:
	FGVirtualBuffer(
		const LString& resource_name,
		const RHIResDesc& res_desc
	);

	~FGVirtualBuffer() override
	{

	};
};


class FGVirtualTexture : public FGVirtualRes
{
	RHITextureDesc mTextureDesc;
public:
	FGVirtualTexture(
		const LString& resource_name,
		const RHIResDesc& res_desc,
		const RHITextureDesc& texture_desc
	);

	~FGVirtualTexture()
	{
	};
	const RHITextureDesc& GetTextureDesc() const
	{
		return mTextureDesc;
	}
};

struct FGVirtualResView
{
	ViewDesc mViewDesc;
	LString Name;
	FGVirtualRes* VirtualRes = nullptr;
	RHIViewPtr mView = nullptr;
};

enum AdaptoHeaprLevel
{
	HEAP_LEVEL_FLAG_1,
	HEAP_LEVEL_FLAG_2
};

}

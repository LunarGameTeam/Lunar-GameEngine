#pragma once

#include "core/foundation/container.h"

#include "render/render_config.h"
#include "render/rhi/rhi_pch.h"

#include <boost/container_hash/hash.hpp>


namespace luna::render
{

inline size_t Alignment(size_t size, size_t alignment)
{
	return (size + (alignment - 1)) & ~(alignment - 1);
}

enum class VertexElementUsage : uint8_t
{
	UsagePosition = 0,
	UsageBlendWeight = 1,
	UsageNormal = 2,
	UsageDiffuse = 3,
	UsageTangent = 4,
	UsageColor = 5,
	UsageTexture0 = 6,
	UsageTexture1 = 7,
	UsageTexture2 = 8,
	UsageTexture3 = 9,
	UsageInstanceMessage = 10,
	UsageMax,
};

enum class VertexElementType : uint8_t
{
	Float, // 4字节
	Short, // 2字节
	UShort, //2字节
	Byte, //1字节
	UByte, // 1字节
	Int, // 4字节	
};

enum class VertexElementInstanceType : uint8_t
{
	PerVertex, // 每顶点
	PerInstance // 每实例	
};


struct RHIVertexLayoutElement
{
	RHIVertexLayoutElement(VertexElementType elementType, VertexElementUsage usage, uint8_t elementCount) :
		mElementType(elementType),
		mUsage(usage),
		mElementCount(elementCount)
	{
		uint8_t elementSize = 0;
		switch (mElementType)
		{
		case luna::render::VertexElementType::Float:
			elementSize = 4;
			break;
		case luna::render::VertexElementType::Short:
			elementSize = 2;
			break;
		case luna::render::VertexElementType::UShort:
			elementSize = 2;
			break;
		case luna::render::VertexElementType::Byte:
			elementSize = 1;
			break;
		case luna::render::VertexElementType::UByte:
			elementSize = 1;
			break;
		case luna::render::VertexElementType::Int:
			elementSize = 4;
			break;
		default:
			break;
		}
		mSize = elementSize * mElementCount;
	}
	size_t Hash() const 
	{
		size_t result = 0;
		boost::hash_combine(result, mElementType);
		boost::hash_combine(result, mElementCount);
		boost::hash_combine(result, mUsage);
		boost::hash_combine(result, mOffset);
		boost::hash_combine(result, mBufferSlot);
		boost::hash_combine(result, mInstanceUsage);
		return result;
	}

	VertexElementType mElementType;
	uint8_t mElementCount;
	VertexElementUsage mUsage;
	uint32_t mOffset;
	uint32_t mSize = 0;
	uint8_t mBufferSlot = 0;
	VertexElementInstanceType mInstanceUsage;
};

struct RHIVertexLayout
{
	RHIVertexLayout()
	{
		mSize.resize(16);
		for (int i = 0; i < 16; ++i)
		{
			mSize[i] = 0;
		}
	}
	void AddVertexElement(VertexElementType type, VertexElementUsage usage, uint8_t elementCount,uint8_t bufferSlot,VertexElementInstanceType instanceUsage)
	{
		mElements.emplace_back(type, usage, elementCount);
		mElements.back().mOffset = (uint32_t) mSize[bufferSlot];
		mElements.back().mBufferSlot = bufferSlot;
		mElements.back().mInstanceUsage = instanceUsage;
		mSize[bufferSlot] += (uint32_t)Alignment(mElements.back().mSize, 8);
		mDirty = true;
	}

	size_t Hash()
	{
		if (mDirty)
		{
			mHash = 0;
			for (auto& element : mElements)
			{
				boost::hash_combine(mHash, element.Hash());
			}
			mDirty = false;
		}
		return mHash;
	}

	LArray<size_t> GetSize() const
	{
		return mSize;
	}

	bool mDirty = true;
	size_t mHash = 0;
	LArray<size_t> mSize;
	LArray<RHIVertexLayoutElement> mElements;
};


}
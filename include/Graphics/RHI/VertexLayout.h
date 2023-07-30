#pragma once

#include "Core/Foundation/Container.h"

#include "Graphics/RenderConfig.h"
#include "Graphics/RHI/RHIPch.h"

#include <boost/container_hash/hash.hpp>


namespace luna::graphics
{

inline size_t Alignment(size_t size, size_t alignment)
{
	return (size + (alignment - 1)) & ~(alignment - 1);
}

enum class VertexElementUsage : uint8_t
{
	UsagePosition = 0,
	UsageColor,
	UsageNormal,
	UsageTangent,
	UsageTexture0,
	UsageTexture1,
	UsageTexture2,
	UsageTexture3,
	UsageBlendIndex,
	UsageBlendWeight,
	UsageInstanceMessage,
	UsageDiffuse,
	UsageMax,
};

enum class VertexElementType : uint8_t
{
	Float, // 4�ֽ�
	Short, // 2�ֽ�
	UShort, //2�ֽ�
	Byte, //1�ֽ�
	UByte, // 1�ֽ�
	Int, // 4�ֽ�	
};

enum class VertexElementInstanceType : uint8_t
{
	PerVertex, // ÿ����
	PerInstance // ÿʵ��	
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
		case luna::graphics::VertexElementType::Float:
			elementSize = 4;
			break;
		case luna::graphics::VertexElementType::Short:
			elementSize = 2;
			break;
		case luna::graphics::VertexElementType::UShort:
			elementSize = 2;
			break;
		case luna::graphics::VertexElementType::Byte:
			elementSize = 1;
			break;
		case luna::graphics::VertexElementType::UByte:
			elementSize = 1;
			break;
		case luna::graphics::VertexElementType::Int:
			elementSize = 4;
			break;
		default:
			break;
		}
		mSize = elementSize * mElementCount;
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
	void AddVertexElement(VertexElementType type, VertexElementUsage usage, uint8_t elementCount,uint8_t bufferSlot, VertexElementInstanceType instanceUsage)
	{
		mElements.emplace_back(type, usage, elementCount);
		mElements.back().mOffset = (uint32_t) mSize[bufferSlot];
		mElements.back().mBufferSlot = bufferSlot;
		mElements.back().mInstanceUsage = instanceUsage;
		mSize[bufferSlot] += (uint32_t)Alignment(mElements.back().mSize, 8);
		mDirty = true;
	}

	LArray<size_t> GetSize() const
	{
		return mSize;
	}

	bool mDirty = true;
	LArray<size_t> mSize;
	LArray<RHIVertexLayoutElement> mElements;
};


}
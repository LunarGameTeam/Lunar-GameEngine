#pragma once

#include "render/pch.h"
#include "render/rhi/rhi_pch.h"


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
	UsageTexture0 = 4,
	UsageTangent = 5,
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


struct InputVertexLayoutElement
{
	InputVertexLayoutElement(VertexElementType elementType, VertexElementUsage usage, uint8_t elementCount) :
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

	VertexElementType mElementType;
	uint8_t mElementCount;
	VertexElementUsage mUsage;
	uint32_t mOffset;
	uint32_t mSize = 0;
};


struct InputVertexLayout
{
	void AddVertexElement(VertexElementType type, VertexElementUsage usage, uint8_t elementCount)
	{
		auto offset = GetSize();
		mElements.emplace_back(type, usage, elementCount);
		mElements.back().mOffset = offset;
	}

	uint32_t GetSize() const
	{
		uint32_t size = 0;
		for (const InputVertexLayoutElement& element : mElements)
		{
			size += (uint32_t)Alignment(element.mSize, 8);
		}
		return size;
	}

	size_t mSize;
	std::vector<InputVertexLayoutElement> mElements;
};


}
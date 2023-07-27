#pragma once
#include "Graphics/RenderConfig.h"
#include "Core/CoreMin.h"

#include "Graphics/RHI/RHITypes.h"
#include "Graphics/RHI/RHIShader.h"

#include "Graphics/RenderTypes.h"
#include "Core/Object/SharedObject.h"


namespace luna::graphics
{

// Shader Buffer的封装，根据CBuffer的Desc创建，内部封装了RHIResource和RHIView
// 提供快捷设置数据和Commit接口
struct RENDER_API ShaderCBuffer : NoCopy
{
	ShaderCBuffer(const RHICBufferDesc& cbDesc);
	ShaderCBuffer(RHIBufferUsage usage, uint32_t size);


	template<typename T>
	void Set(const LString& name, const T& val, uint32_t arrayIdx = 0, size_t extra_offset = 0)
	{
		CBufferVar& var = mVars[name.Hash()];
		size_t elementSize = 0;
		if (var.mIsArray)
			elementSize = var.mElementSize;
		auto offset = var.mOffset + elementSize * arrayIdx;
		LUNA_ASSERT(offset + extra_offset + sizeof(T) <= mData.size());
		*((T*)(mData.data() + offset + extra_offset)) = val;
	}

	template<typename T>
	void Set(size_t extra_offset, const T& val)
	{
		LUNA_ASSERT(extra_offset + sizeof(T) <= mData.size());
		*((T*)(mData.data() + extra_offset)) = val;
	}

	void SetData(const LString& name, void* data, uint32_t dataSize)
	{
		auto offset = mVars[name.Hash()].mOffset;
		LUNA_ASSERT(offset + dataSize <= mData.size());
		memcpy(mData.data() + offset, data, dataSize);
	}

	void SetData(uint32_t offset, void* data, uint32_t dataSize)
	{
		LUNA_ASSERT(offset + dataSize <= mData.size());
		memcpy(mData.data() + offset, data, dataSize);
	}

	void Commit();

	LMap<size_t, CBufferVar> mVars;
	LArray<byte>             mData;
	RHIResourcePtr           mRes;
	RHIViewPtr               mView;
};

//把ShaderParam打包，Param名字 + Texture View，或者是放ShaderParamBuffer
struct RENDER_API PackedParams : NoCopy
{
	void PushShaderParam(RHIBindPoint id, RHIViewPtr view);
	LArray<std::pair<RHIBindPoint, RHIViewPtr>> mParams;
};

}
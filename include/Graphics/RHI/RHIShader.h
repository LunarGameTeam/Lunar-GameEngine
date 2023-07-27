#pragma once
#include "Graphics/RenderConfig.h"
#include "Graphics/RHI/RHITypes.h"
#include "Graphics/RHI/RHIPch.h"
#include "Graphics/RHI/RHIPtr.h"
#include "Graphics/RHI/RHIBindingSetLayout.h"

namespace luna::graphics
{

#define PARAM_ID(name) static luna::graphics::ShaderParamID ParamID_##name = luna::LString(#name).Hash();

struct RHIShaderDesc
{
	LString       mName;
	LString       mContent;
	RHIShaderType mType;
	LString       mEntryPoint;
	LArray<RhiShaderMacro> mShaderMacros;
};

struct CBufferVar
{
	LString                  mName = "";
	size_t                   mOffset = 0;
	size_t                   mSize = 0;
	bool                     mIsArray = false;
	bool                     mIsStruct = false;
	LMap<size_t, CBufferVar> mStructVars;
	size_t                   mElementSize = 0;
};

struct RHICBufferDesc
{
	LString                  mName = "";
	size_t                   mSize = 0;
	LMap<size_t, CBufferVar> mVars;
};

template<typename T>
void MemSet(void* p, const T& val)
{
	*((T*)p) = val;
}

using ShaderParamID = size_t;

using BindPointIt = std::unordered_map<ShaderParamID, RHIBindPoint>::iterator;


class RENDER_API RHIShaderBlob : public RHIObject
{
public:
	RHIShaderDesc mDesc;
	
	LMap<ShaderParamID, RHICBufferDesc> mUniformBuffers;
	std::unordered_map<ShaderParamID, RHIBindPoint>    mBindPoints;

	

	auto GetBindPoint(ShaderParamID id)
	{
		return mBindPoints.find(id);
	};

	bool HasBindPoint(ShaderParamID id)
	{
		return mBindPoints.find(id) != mBindPoints.end();
	}

	RHICBufferDesc& GetUniformBuffer(ShaderParamID name)
	{
		return mUniformBuffers[name];
	}

	bool HasUniformBuffer(ShaderParamID name)
	{
		if (mUniformBuffers.find(name) == mUniformBuffers.end())
		{
			return false;
		}
		return true;
	}

	RHIShaderBlob(const RHIShaderDesc& desc)
	{
		mDesc = desc;
	};

	RHIShaderDesc GetDesc()
	{
		return mDesc;
	}

	~RHIShaderBlob() override
	{
	};
};


}

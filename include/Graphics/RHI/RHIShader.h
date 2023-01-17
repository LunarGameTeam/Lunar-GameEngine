#pragma once
#include "Graphics/RenderConfig.h"
#include "Graphics/RHI/RHITypes.h"
#include "Graphics/RHI/RHIPch.h"
#include "Graphics/RHI/RHIPtr.h"
#include "Graphics/RHI/RHIBindingSetLayout.h"

namespace luna::render
{



#define PARAM_ID(name) static luna::render::ShaderParamID ParamID_##name = luna::LString(#name).Hash();

struct RHIShaderDesc
{
	LString       mName;
	LString       mContent;
	RHIShaderType mType;
	LString       mEntryPoint;
};

struct ConstantBufferVar
{
	size_t mOffset = 0;
	size_t mSize = 0;
};

struct RHIConstantBufferDesc
{
	size_t mBufferSize = 0;
	LUnorderedMap<LString, ConstantBufferVar> mVars;
};

using ShaderParamID = size_t;


class RENDER_API RHIShaderBlob : public RHIObject
{
public:
	RHIShaderDesc mDesc;

	std::unordered_map<LString, RHIConstantBufferDesc>   mUniformBuffers;
	std::unordered_map<ShaderParamID, RHIBindPoint>            mBindPoints;

	RHIBindPoint GetBindPoint(ShaderParamID id)
	{
		RHIBindPoint bind_point;
		auto it = mBindPoints.find(id);
		if (it == mBindPoints.end())
		{
			bind_point.mSlot = -1;
			return bind_point;
		}
		return it->second;
	};

	bool HasBindPoint(ShaderParamID id)
	{
		return mBindPoints.find(id) != mBindPoints.end();
	}

	RHIConstantBufferDesc& GetUniformBuffer(const LString& name)
	{
		return mUniformBuffers[name];
	}

	bool HasUniformBuffer(const LString& name)
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

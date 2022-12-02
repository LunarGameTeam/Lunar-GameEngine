#pragma once
#include "render/pch.h"
#include "render/rhi/rhi_types.h"
#include "render/rhi/rhi_pch.h"
#include "render/rhi/rhi_ptr.h"
#include "render/rhi/rhi_binding_set_layout.h"

namespace luna::render
{


struct RHIShaderDesc
{
	LString       mName; //shader名称
	LString       mContent; //shader内容
	RHIShaderType mType; //shader类型
	LString       mEntryPoint; //shader入口函数
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

	// Pipeline 的 CBuffer 描述。方便Binding
	std::unordered_map<LString, RHIConstantBufferDesc>   mUniformBuffers;
	// Pipeline 的资源输入信息
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

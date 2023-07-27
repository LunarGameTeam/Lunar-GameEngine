#pragma once

#include "Core/Math/Math.h"
#include "Core/Asset/Asset.h"
#include "Core/Foundation/String.h"
#include "Graphics/RenderConfig.h"
#include "Graphics/RHI/RHIShader.h"


namespace luna::graphics
{

class RENDER_API ShaderMacro : public LObject
{
	RegisterTypeEmbedd(ShaderMacro, LObject)
public:
	ShaderMacro()
	{
	};
	LString mMacroName;
	LString mMacroValue;
};

class LShaderInstance
{
public:
	LShaderInstance(RHIShaderType type, const LString& shaderName) :mType(type), mShaderName(shaderName)
	{
	}

	bool HasBindPoint(ShaderParamID id) const
	{
		return mRhiShader->HasBindPoint(id);
	}

	RHIBindPoint GetBindPoint(ShaderParamID id) const
	{
		return mRhiShader->GetBindPoint(id)->second;
	};

	RHICBufferDesc GetConstantBufferDesc(ShaderParamID name)
	{
		return mRhiShader->GetUniformBuffer(name);
		RHICBufferDesc empty;
		return empty;
	}

	void Init(
		const LString &content,
		LArray<ShaderMacro*> shaderMacros = LArray<ShaderMacro*>()
	);

	RHIShaderBlobPtr GetRhiShader() { return mRhiShader; }

	RHIBindingSetLayoutPtr mLayout;
private:
	RHIShaderBlobPtr mRhiShader;

	size_t mShaderCacheId;

	RHIShaderType mType;

	LString mShaderName;

	bool mInit = false;
};

class RENDER_API ShaderAsset : public TextAsset
{
	RegisterTypeEmbedd(ShaderAsset, TextAsset)
public:
	ShaderAsset()
	{
	}
	void OnAssetFileRead(LSharedPtr<JsonDict> meta, LSharedPtr<LFile> file) override;

	SharedPtr<LShaderInstance> GenerateShaderInstance(RHIShaderType shaderType, const LArray<ShaderMacro*>& shaderMacros);
};


}

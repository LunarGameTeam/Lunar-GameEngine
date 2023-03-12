#pragma once

#include "Core/Math/Math.h"
#include "Core/Asset/Asset.h"
#include "Core/Foundation/String.h"
#include "Graphics/RenderConfig.h"
#include "Graphics/RHI/RHIShader.h"


namespace luna::render
{

class RENDER_API ShaderAsset : public TextAsset
{
	RegisterTypeEmbedd(ShaderAsset, TextAsset)
public:
	ShaderAsset()
	{
	}

	void OnAssetFileRead(LSharedPtr<JsonDict> meta, LSharedPtr<LFile> file) override;


	bool HasBindPoint(ShaderParamID id) const 
	{
		if (mVS->HasBindPoint(id))
			return true;
		return mPS->HasBindPoint(id);
	}
	RHIBindPoint GetBindPoint(ShaderParamID id) const
	{
		if(mVS->HasBindPoint(id))
			return mVS->GetBindPoint(id)->second;
		return mPS->GetBindPoint(id)->second;
	};

	RHICBufferDesc GetConstantBufferDesc(ShaderParamID name)
	{
		if (mVS->HasUniformBuffer(name))
		{
			return mVS->GetUniformBuffer(name);
		}
		else if(mPS->HasUniformBuffer(name))
		{
			return mPS->GetUniformBuffer(name);
		}
		RHICBufferDesc empty;
		return empty;
	}

	RHIShaderBlobPtr& GetVertexShader() { return mVS; }

	size_t GetVsId() { return mVsId; };

	RHIShaderBlobPtr& GetPixelShader() { return mPS; }

	size_t GetPsId() { return mPsId; };
	RHIBindingSetLayoutPtr mLayout;

protected:
	void Init();
private:
	RHIShaderBlobPtr mVS;
	size_t mVsId;
	RHIShaderBlobPtr mPS;
	size_t mPsId;
	bool mInit = false;
};
}

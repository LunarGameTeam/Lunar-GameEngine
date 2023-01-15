#pragma once

#include "core/math/math.h"
#include "core/asset/asset.h"
#include "core/foundation/string.h"
#include "render/render_config.h"
#include "render/rhi/rhi_shader.h"


namespace luna::render
{

class RENDER_API ShaderAsset : public TextAsset
{
	RegisterTypeEmbedd(ShaderAsset, TextAsset)
public:
	ShaderAsset()
	{
	}

	void OnAssetFileRead(LSharedPtr<Dictionary> meta, LSharedPtr<LFile> file) override;


	bool HasBindPoint(ShaderParamID id) const 
	{
		if (mVS->HasBindPoint(id))
			return true;
		return mPS->HasBindPoint(id);
	}
	RHIBindPoint GetBindPoint(ShaderParamID id) const
	{
		if(mVS->HasBindPoint(id))
			return mVS->GetBindPoint(id);
		return mPS->GetBindPoint(id);
	};

	RHIConstantBufferDesc GetConstantBufferDesc(const LString& name)
	{
		if (mVS->HasUniformBuffer(name))
		{
			return mVS->GetUniformBuffer(name);
		}
		else if(mPS->HasUniformBuffer(name))
		{
			return mPS->GetUniformBuffer(name);
		}
		RHIConstantBufferDesc empty;
		return empty;
	}

	RHIShaderBlobPtr& GetVertexShader() { return mVS; }

	RHIShaderBlobPtr& GetPixelShader() { return mPS; }

protected:
	void Init();
private:
	RHIShaderBlobPtr mVS;
	RHIShaderBlobPtr mPS;
	bool mInit = false;
};
}

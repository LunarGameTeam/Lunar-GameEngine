#pragma once

#include "core/math/math.h"
#include "core/asset/asset.h"
#include "core/misc/string.h"
#include "render/pch.h"
#include "render/rhi/rhi_shader.h"


namespace luna::render
{
class RENDER_API ShaderAsset : public LTextAsset
{
	RegisterTypeEmbedd(ShaderAsset, LTextAsset)
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

	RHIConstantBufferDesc& GetConstantBufferDesc(const LString& name)
	{
		return mVS->GetUniformBuffer(name);
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

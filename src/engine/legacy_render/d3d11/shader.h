#pragma once

#include "legacy_render/interface/i_shader.h"
#include "legacy_render/private_render.h"
#include "core/misc/container.h"
#include "core/asset/asset.h"
#include <d3d11.h>

namespace luna
{
namespace legacy_render
{

enum class ShaderType
{
	None,
	VertexShader,
	PixelShader
};

class LEGACY_RENDER_API Dx11Shader : public LBasicAsset, public IShader
{
public:

	bool Init() override;
	bool Clean() override;
	void SetWVPMatrix(const LMatrix4f &w, const LMatrix4f &v, const LMatrix4f &p) override;
	void SetParameterInt(const LString &name, int32_t value) override;
	void SetParameterUInt(const LString &name, uint32_t value) override;
	void SetParameterFloat(const LString &name, float value) override;
	void SetParameterFloat3(const LString &name, const LVector3f &value) override;
	void SetParameterFloat4(const LString &name, const LVector4f &value) override;
	void SetParameterMatrix3(const LString &name, const LMatrix3f &value) override;
	void SetParameterMatrix4(const LString &name, const LMatrix4f &value) override;


	void Bind() override;


	void OnAssetFileLoad(LSharedPtr<Dictionary> meta, LSharedPtr<LFile> file) override;

private:
	ShaderType m_shader_type = ShaderType::None;

	ID3D11VertexShader *m_vertexShader;
	ID3D11PixelShader *m_pixelShader;
	ID3D11InputLayout *m_layout;
	ID3D11Buffer *m_cb0_buffer;
	ID3D11SamplerState *m_sampleState;

	LMap<LString, ID3D11Buffer *> m_buffer_map;
	
};

}
}

#pragma once

#include "legacy_render/interface/i_shader.h"

namespace luna
{
namespace legacy_render
{

class Dx11Shader : public IShader
{
public:

	bool Init() override;


	bool Clean() override;


	void SetParameterInt(const LString &name, int32_t value) override;


	void SetParameterUInt(const LString &name, uint32_t value) override;


	void SetParameterFloat(const LString &name, float value) override;


	void SetParameterFloat3(const LString &name, const LVector3f &value) override;


	void SetParameterFloat4(const LString &name, const LVector4f &value) override;


	void SetParameterMatrix3(const LString &name, const LMatrix3f &value) override;


	void SetParameterMatrix4(const LString &name, const LMatrix4f &value) override;

	LString m_vs_path;
	LString m_ps_path;

};

}
}

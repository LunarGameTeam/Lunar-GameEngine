#pragma once

#include "core/math/math.h"
#include "core/misc/string.h"



namespace luna
{
namespace legacy_render
{

struct MatrixBufferType
{
	LMatrix4f world;
	LMatrix4f view;
	LMatrix4f projection;
};

class IShader
{
public:
	virtual bool Init() = 0;
	virtual bool Clean() = 0;
	virtual void SetParameterInt(const LString &name, int32_t value) = 0;
	virtual void SetParameterUInt(const LString &name, uint32_t value) = 0;
	virtual void SetParameterFloat(const LString& name,float value) = 0;
	virtual void SetParameterFloat3(const LString &name, const LVector3f &value) = 0;
	virtual void SetParameterFloat4(const LString &name, const LVector4f &value) = 0;
	virtual void SetParameterMatrix3(const LString &name, const LMatrix3f &value) = 0;
	virtual void SetParameterMatrix4(const LString &name, const LMatrix4f &value) = 0;
};

}
}

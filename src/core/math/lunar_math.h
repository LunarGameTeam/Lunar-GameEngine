#pragma once
#include "core/core_library.h"
#ifdef  _MSC_VER
#include<DirectXMath.h>
#endif

#ifndef CORE_API
#define CORE_API __declspec( dllexport )//宏定义
#endif
namespace luna
{
class CORE_API LunarMatrix3x3
{
	DirectX::XMFLOAT3X3 matrix_value;
public:
	LunarMatrix3x3();
	const DirectX::XMFLOAT3X3 &GetMatrixValue() const
	{
		return matrix_value;
	}
	LunarMatrix3x3 operator*(const LunarMatrix3x3 &matrix_right);
	LunarMatrix3x3 TransposeMatrix();
	LunarMatrix3x3 InverseMatrix();
};

class CORE_API LunarMatrix4x4
{
	DirectX::XMFLOAT4X4 matrix_value;
public:
	LunarMatrix4x4();
	const DirectX::XMFLOAT4X4 &GetMatrixValue() const
	{
		return matrix_value;
	}
	LunarMatrix4x4 operator*(const LunarMatrix4x4 &matrix_right);
	LunarMatrix4x4 TransposeMatrix() const;
	LunarMatrix4x4 InverseMatrix() const;
};

class CORE_API LunarVector2
{
	DirectX::XMFLOAT2 vector_value;
public:
	LunarVector2();
	LunarVector2(const float &x, const float &y);
	LunarVector2 operator+(const LunarVector2 &vector_right);
	LunarVector2 operator-(const LunarVector2 &vector_right);
	LunarVector2 operator*(const LunarVector2 &vector_right);
	float DotValue(const LunarVector2 &vector_right);
	float CrossVec(const LunarVector2 &vector_right);
	LunarVector2 NormalizeVec();
	inline void SetValue(const float &x, const float &y)
	{
		vector_value.x = x;
		vector_value.y = y;
	}
};

struct CORE_API LunarVector3
{
	DirectX::XMFLOAT3 vector_value;
public:
	LunarVector3();
	LunarVector3(const float &x, const float &y, const float &z);
	LunarVector3 operator+(const LunarVector3 &vector_right);
	LunarVector3 operator-(const LunarVector3 &vector_right);
	LunarVector3 operator*(const LunarVector3 &vector_right);
	LunarVector3 operator*(const LunarMatrix3x3 &matrix_right);
	LunarVector3 operator*(const LunarMatrix4x4 &matrix_right);
	inline float X()
	{
		return vector_value.x;
	}
	inline float Y()
	{
		return vector_value.y;
	}
	inline float Z()
	{
		return vector_value.z;
	}
	float DotValue(const LunarVector3 &vector_right);
	LunarVector3 CrossVec(const LunarVector3 &vector_right);
	LunarVector3 NormalizeVec();
	inline void SetValue(const float &x, const float &y, const float &z)
	{
		vector_value.x = x;
		vector_value.y = y;
		vector_value.z = z;
	}
};

class CORE_API LunarVector4
{
	DirectX::XMFLOAT4 vector_value;
public:
	LunarVector4();
	LunarVector4(const float &x, const float &y, const float &z, const float &w);
	LunarVector4 operator+(const LunarVector4 &vector_right);
	LunarVector4 operator-(const LunarVector4 &vector_right);
	LunarVector4 operator*(const LunarVector4 &vector_right);
	LunarVector4 operator*(const LunarMatrix4x4 &matrix_right);
	float DotValue3(const LunarVector4 &vector_right);
	float DotValue4(const LunarVector4 &vector_right);
	LunarVector3 CrossVec(const LunarVector3 &vector_right);
	LunarVector3 CrossVec(const LunarVector4 &vector_right);
	LunarVector3 NormalizeVec3();
	LunarVector4 NormalizeVec4();
	inline LunarVector3 GetVector3() const
	{
		return LunarVector3(vector_value.x, vector_value.y, vector_value.z);
	}
	const DirectX::XMFLOAT4 &GetVectorValue() const
	{
		return vector_value;
	}
};

class CORE_API LunarUint4
{
	DirectX::XMUINT4 vector_value;
public:
	LunarUint4();
	LunarUint4(const uint32_t &x, const uint32_t &y, const uint32_t &z, const uint32_t &w);
	LunarUint4 operator+(const LunarUint4 &vector_right);
	LunarUint4 operator-(const LunarUint4 &vector_right);
	LunarUint4 operator*(const LunarUint4 &vector_right);
	/*uint32_t DotValue4(const LunarVector4& vector_right);
	inline LunarVector3 GetVector3() const
	{
		return LunarVector3(vector_value.x, vector_value.y, vector_value.z);
	}*/
	const DirectX::XMUINT4 &GetVectorValue() const
	{
		return vector_value;
	}
};
}

#pragma once
#include "core/core_module.h"
#ifdef  _MSC_VER
#include<DirectXMath.h>
#endif

namespace luna
{
	class LunarMatrix3x3
	{
		DirectX::XMFLOAT3X3 matrix_value;
	public:
		LunarMatrix3x3();
		const DirectX::XMFLOAT3X3& GetMatrixValue() const
		{
			return matrix_value;
		}
		LunarMatrix3x3 operator*(const LunarMatrix3x3& matrix_right);
		LunarMatrix3x3 TransposeMatrix();
		LunarMatrix3x3 InverseMatrix();
	};

	class LunarMatrix4x4
	{
		DirectX::XMFLOAT4X4 matrix_value;
	public:
		LunarMatrix4x4();
		const DirectX::XMFLOAT4X4& GetMatrixValue() const
		{
			return matrix_value;
		}
		LunarMatrix4x4 operator*(const LunarMatrix4x4& matrix_right);
		LunarMatrix4x4 TransposeMatrix();
		LunarMatrix4x4 InverseMatrix();
	};

	class LunarVector2
	{
		DirectX::XMFLOAT2 vector_value;
	public:
		LunarVector2();
		LunarVector2(const float& x, const float& y);
		LunarVector2 operator+(const LunarVector2& vector_right);
		LunarVector2 operator-(const LunarVector2& vector_right);
		LunarVector2 operator*(const LunarVector2& vector_right);
		float DotValue(const LunarVector2& vector_right);
		float CrossVec(const LunarVector2& vector_right);
		LunarVector2 NormalizeVec();
		inline void SetValue(const float& x, const float& y)
		{
			vector_value.x = x;
			vector_value.y = y;
		}
	};

	class LunarVector3
	{
		DirectX::XMFLOAT3 vector_value;
	public:
		LunarVector3();
		LunarVector3(const float& x, const float& y, const float& z);
		LunarVector3 operator+(const LunarVector3& vector_right);
		LunarVector3 operator-(const LunarVector3& vector_right);
		LunarVector3 operator*(const LunarVector3& vector_right);
		LunarVector3 operator*(const LunarMatrix3x3& matrix_right);
		LunarVector3 operator*(const LunarMatrix4x4& matrix_right);
		float DotValue(const LunarVector3& vector_right);
		LunarVector3 CrossVec(const LunarVector3& vector_right);
		LunarVector3 NormalizeVec();
		inline void SetValue(const float& x, const float& y, const float& z)
		{
			vector_value.x = x;
			vector_value.y = y;
			vector_value.z = z;
		}
	};

	class LunarVector4
	{
		DirectX::XMFLOAT4 vector_value;
	public:
		LunarVector4();
		LunarVector4(const float& x, const float& y, const float& z, const float& w);
		LunarVector4 operator+(const LunarVector4& vector_right);
		LunarVector4 operator-(const LunarVector4& vector_right);
		LunarVector4 operator*(const LunarVector4& vector_right);
		LunarVector4 operator*(const LunarMatrix4x4& matrix_right);
		float DotValue3(const LunarVector4& vector_right);
		float DotValue4(const LunarVector4& vector_right);
		LunarVector3 CrossVec(const LunarVector3& vector_right);
		LunarVector3 CrossVec(const LunarVector4& vector_right);
		LunarVector3 NormalizeVec3();
		LunarVector4 NormalizeVec4();
		inline LunarVector3 GetVector3() const
		{
			return LunarVector3(vector_value.x, vector_value.y, vector_value.z);
		}
	};

}

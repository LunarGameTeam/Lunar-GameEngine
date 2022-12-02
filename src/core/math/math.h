#pragma once

#include <Eigen/Dense>

#include <numbers>

namespace luna
{
using LVector2f = Eigen::Vector2f;
using LVector3f = Eigen::Vector3f;
using LVector4f = Eigen::Vector4f;
using LQuaternion = Eigen::Quaternionf;
using LTransform = Eigen::Transform<float, 3, Eigen::Affine, Eigen::RowMajor>;
using LMatrix3f = Eigen::Matrix<float, 3, 3, Eigen::RowMajor>;
using LMatrix4f = Eigen::Matrix<float, 4, 4, Eigen::RowMajor>;
using LAABB = Eigen::AlignedBox<float, 3>;

class CORE_API LMath
{
public:
	static void GenPerspectiveFovLHMatrix(LMatrix4f &m, float fovy, float aspect, float zn, float zf);
	static void GenOrthoLHMatrix(LMatrix4f &m, float w, float h, float zn, float zf);
	static LQuaternion FromEuler(const LVector3f &euler);
	static LVector3f ToEuler(const LQuaternion& quat);

	inline static LVector3f xyz(const LVector4f& val) { return LVector3f(val.x(), val.y(), val.z()); }
	inline static LVector4f xyzw(const LVector3f& val, float w = 1.0f) { return LVector4f(val.x(), val.y(), val.z(), w); }
};


struct CORE_API LFrustum
{
	/* 1 —————— 2
	*  |        |
	*  |        |
	*  4 —————— 3
	*/

	LVector4f near_pos[4];
	LVector4f far_pos[4];

	LFrustum Multiple(const LMatrix4f& mat)
	{
		LFrustum f;
		for(int i = 0; i < 4 ; i++)			
			f.near_pos[i] = mat * near_pos[i];
		for (int i = 0; i < 4; i++)
			f.far_pos[i] = mat * far_pos[i];
		return f;		
	}
};

}
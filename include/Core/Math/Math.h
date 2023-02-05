#pragma once

#include "Core/CoreConfig.h"

#include <Eigen/Dense>

#include <numbers>

namespace luna
{

using LVector2f = Eigen::Vector2f;
using LVector3f = Eigen::Vector3f;
using LVector3d = Eigen::Vector3d;
using LVector4f = Eigen::Vector4f;
using LVector4d = Eigen::Vector4d;
using LQuaternion = Eigen::Quaternionf;
using LQuaterniond = Eigen::Quaterniond;
using LTransform = Eigen::Transform<float, 3, Eigen::Affine, Eigen::RowMajor>;
using LTransformd = Eigen::Transform<double, 3, Eigen::Affine, Eigen::RowMajor>;
using LMatrix3f = Eigen::Matrix<float, 3, 3, Eigen::RowMajor>;
using LMatrix4f = Eigen::Matrix<float, 4, 4, Eigen::RowMajor>;
using LAABB = Eigen::AlignedBox<float, 3>;

class CORE_API LMath
{
public:
	static void GenPerspectiveFovLHMatrix(LMatrix4f &m, float fovy, float aspect, float zn, float zf);
	static void GenOrthoLHMatrix(LMatrix4f &m, float w, float h, float zn, float zf);

	template<typename T>
	static T ToAngle(T val)
	{
		return val / std::numbers::pi_v<T> *(T)180;
	}

	template<typename T>
	static T ToRad(T val)
	{
		return val * std::numbers::pi_v<T> / (T)180;
	}
	static LQuaternion FromEuler(const LVector3f &euler);
	static LVector3f ToEuler(const LQuaternion& quat);
	static void MatrixDecompose(const LMatrix4f& m, LVector3f& translation, LQuaternion& rotation, LVector3f& scale);
	static LMatrix4f MatrixCompose(LVector3f& translation, LQuaternion& rotation, LVector3f& scale);

	static LQuaternion AngleAxisf(float val, const LVector3f& axis);
	inline static LVector3f ToVector3f(const LVector4f& val) { return LVector3f(val.x(), val.y(), val.z()); }
	inline static LVector4f ToVector4f(const LVector3f& val, float w = 1.0f) { return LVector4f(val.x(), val.y(), val.z(), w); }

	static LVector3f GetMatrixTranslaton(const LMatrix4f& mat)
	{
		LTransform transform(mat);
		return transform.translation();
	}

	static LQuaternion GetMatrixRotation(const LMatrix4f& mat)
	{
		LTransform transform(mat);
		return LQuaternion(transform.rotation());		
	}
};

struct LRay;
struct LAabb;

//基于Eigen AABB的封装
struct CORE_API LAabb : public Eigen::AlignedBox<float, 3>
{
	using Base = Eigen::AlignedBox<float, 3>;

	LAabb(const LVector3f& _min, const LVector3f& _max) :
		Eigen::AlignedBox<float, 3>(_min, _max)
	{

	}

	bool IntersectRay(const LRay& ray);


};



struct CORE_API LRay : public Eigen::ParametrizedLine<float, 3>
{
	using Base = Eigen::ParametrizedLine<float, 3>;


	LRay(const LVector3f origin, const LVector3f direction)
		: Base(origin, direction)
	{
		
	};

};

struct CORE_API LFrustum
{
	/* 1 —————— 2
	*  |        |
	*  |        |
	*  4 —————— 3
	*/

	LVector3f mNearPlane[4];
	LVector3f mFarPlane[4];

	static LFrustum MakeFrustrum(float fovY, float zNear, float zFar, float aspect);
	static LFrustum FromOrth(float zNear, float zFar, float width, float height);

	void Multiple(const LMatrix4f& mat)
	{
		for(int i = 0; i < 4 ; i++)			
			mNearPlane[i] = LMath::ToVector3f(mat * LMath::ToVector4f(mNearPlane[i]));
		for (int i = 0; i < 4; i++)
			mFarPlane[i] = LMath::ToVector3f(mat * LMath::ToVector4f(mFarPlane[i]));
				
	}
};

}
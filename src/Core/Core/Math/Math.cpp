#include "Core/Math/Math.h"

namespace luna
{

void LMath::GenPerspectiveFovLHMatrix(LMatrix4f &m, float fovy, float aspect, float zn, float zf)
{	

	float    SinFov;
	float    CosFov;
	SinFov = (float)sinf(0.5f * fovy);
	CosFov = (float)cosf(0.5f * fovy);

	float Height = CosFov / SinFov;
	float Width = Height / aspect;
	float fRange = zf / (zf - zn);

	m(0, 0) = Width;
	m(0, 1) = 0.0f;
	m(0, 2) = 0.0f;
	m(0, 3) = 0.0f;

	m(1, 0) = 0.0f;
	m(1, 1) = Height;
	m(1, 2) = 0.0f;
	m(1, 3) = 0.0f;

	m(2, 0) = 0.0f;
	m(2, 1) = 0.0f;
	m(2, 2) = fRange;
	m(2, 3) = -fRange * zn;

	m(3, 0) = 0.0f;
	m(3, 1) = 0.0f;
	m(3, 2) = 1.0f;
	m(3, 3) = 0.0f;


}

void LMath::GenOrthoLHMatrix(LMatrix4f &m, float w, float h, float zn, float zf)
{
	float fRange = 1.f / (zf - zn);

	m(0, 0) = 2.0f / w;
	m(0, 1) = 0.0f;
	m(0, 2) = 0.0f;
	m(0, 3) = 0.0f;

	m(1, 0) = 0.0f;
	m(1, 1) = 2.0f / h;
	m(1, 2) = 0.0f;
	m(1, 3) = 0.0f;

	m(2, 0) = 0.0f;
	m(2, 1) = 0.0f;
	m(2, 2) = fRange;
	m(2, 3) = 0.0f;

	m(3, 0) = 0.0f;
	m(3, 1) = 0.0f;
	m(3, 2) = -fRange * zn;
	m(3, 3) = 1.0f;
}


LQuaternion LMath::FromEuler(const LVector3f &euler)
{

	float xEuler = ToRad(euler[0]);
	float yEuler = ToRad(euler[1]);
	float zEuler = ToRad(euler[2]);
	auto ry = Eigen::AngleAxisf(yEuler, LVector3f::UnitY());
	auto rx = Eigen::AngleAxisf(xEuler, LVector3f::UnitX());
	auto rz = Eigen::AngleAxisf(zEuler, LVector3f::UnitZ());
	LQuaternion q = ry * rx * rz;
	q.normalized();
	return q;
}

LVector3f LMath::ToEuler(const LQuaternion& quat)
{
	LVector3f res = quat.toRotationMatrix().eulerAngles(1, 0, 2);
	res = res / std::numbers::pi_v<float> *180.f;	
	std::swap(res.x(), res.y());
	return res;
}

LQuaternion LMath::AngleAxisf(float val, const LVector3f& axis)
{
	return LQuaternion(Eigen::AngleAxisf(val, axis).toRotationMatrix());
}


bool LAabb::IntersectRay(const LRay& ray)
{
	float tmin, tmax;
	using namespace Eigen;
	// This should be precomputed and provided as input
	LVector3f dir = ray.direction();
	LVector3f origin = ray.origin();
	const LVector3f inv_dir(1. / dir(0), 1. / dir(1), 1. / dir(2));
	const std::array<bool, 3> sign = { inv_dir(0) < 0, inv_dir(1) < 0, inv_dir(2) < 0 };
	// http://people.csail.mit.edu/amy/papers/box-jgt.pdf
	// "An Efficient and Robust Ray¨CBox Intersection Algorithm"
	float tymin, tymax, tzmin, tzmax;
	std::array<LVector3f, 2> bounds = { this->min(),this->max() };
	tmin = (bounds[sign[0]](0) - origin(0)) * inv_dir(0);
	tmax = (bounds[1 - sign[0]](0) - origin(0)) * inv_dir(0);
	tymin = (bounds[sign[1]](1) - origin(1)) * inv_dir(1);
	tymax = (bounds[1 - sign[1]](1) - origin(1)) * inv_dir(1);
	if ((tmin > tymax) || (tymin > tmax))
	{
		return false;
	}
	if (tymin > tmin)
	{
		tmin = tymin;
	}
	if (tymax < tmax)
	{
		tmax = tymax;
	}
	tzmin = (bounds[sign[2]](2) - origin(2)) * inv_dir(2);
	tzmax = (bounds[1 - sign[2]](2) - origin(2)) * inv_dir(2);
	if ((tmin > tzmax) || (tzmin > tmax))
	{
		return false;
	}
	if (tzmin > tmin)
	{
		tmin = tzmin;
	}
	if (tzmax < tmax)
	{
		tmax = tzmax;
	}
	// 		if (!((tmin < t1) && (tmax > t0)))
	// 		{
	// 			return false;
	// 		}
	return true;
}

}
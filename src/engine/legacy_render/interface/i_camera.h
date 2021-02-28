#pragma once

#include "core/math/math.h"
#include <Eigen/Geometry>


namespace luna
{
namespace legacy_render
{

class ICamera
{
public:
	virtual const LVector3f &GetPosition() 
	{
		return m_pos;
	}
	virtual const LQuaternion &GetRotation()
	{
		return rotation;
	}
	virtual LMatrix4f& GetViewMatrix() 
	{
		LTransform trans = LTransform::Identity();
		trans.translate(m_pos).rotate(rotation);
		m_view_matrix = trans.inverse().matrix();
		return m_view_matrix;
	}
	virtual LMatrix4f &GetProjectionMatrix()
	{
		m_proj_matrix << 1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, -(zFar + zNear) / (zFar - zNear), -1,
			0, 0, -2 * zNear * zFar / (zFar - zNear), 0;
		return m_proj_matrix;
	}
private:
	float zFar = 1000.f, zNear = 0.1f;
	LMatrix4f m_view_matrix = LMatrix4f::Identity();
	LMatrix4f m_proj_matrix = LMatrix4f::Identity();
	LVector3f m_pos = LVector3f(0, 0, 0);
	LQuaternion rotation = LQuaternion::Identity();
	float m_fov;
};

}
}

#pragma once

#include "core/math/math.h"
#include <Eigen/Geometry>
#include "window/luna_window.h"
#include "core/event/event_subsystem.h"

namespace luna
{

class ICamera
{
public:
	virtual LMatrix4f &GetViewMatrix()
	{
		LTransform trans = LTransform::Identity();
		trans.translate(m_pos).rotate(rotation);
		m_view_matrix = trans.inverse().matrix();
		return m_view_matrix;
	}
	virtual LMatrix4f &GetProjectionMatrix()
	{
		float    SinFov;
		float    CosFov;
		SinFov = (float)sinf(0.5f * m_fov);
		CosFov = (float)cosf(0.5f * m_fov);

		float Height = CosFov / SinFov;
		float Width = Height / AspectRatio;
		float fRange = zFar / (zFar - zNear);

		m_proj_matrix(0, 0) = Width;
		m_proj_matrix(0, 1) = 0.0f;
		m_proj_matrix(0, 2) = 0.0f;
		m_proj_matrix(0, 3) = 0.0f;

		m_proj_matrix(1, 0) = 0.0f;
		m_proj_matrix(1, 1) = Height;
		m_proj_matrix(1, 2) = 0.0f;
		m_proj_matrix(1, 3) = 0.0f;

		m_proj_matrix(2, 0) = 0.0f;
		m_proj_matrix(2, 1) = 0.0f;
		m_proj_matrix(2, 2) = fRange;
		m_proj_matrix(2, 3) = 1.0f;

		m_proj_matrix(3, 0) = 0.0f;
		m_proj_matrix(3, 1) = 0.0f;
		m_proj_matrix(3, 2) = -fRange * zNear;
		m_proj_matrix(3, 3) = 0.0f;

		m_proj_matrix.transposeInPlace();
		return m_proj_matrix;
	}

	GET_SET_REF_CONST(LVector3f, m_pos, Position);
	GET_SET_REF_CONST(LQuaternion, rotation, Rotation);


private:
	float zFar = 1000.f, zNear = 0.1f;
	LMatrix4f m_view_matrix = LMatrix4f::Identity();
	LMatrix4f m_proj_matrix = LMatrix4f::Identity();
	LVector3f m_pos = LVector3f(0, 0, 0);
	LQuaternion rotation = LQuaternion::Identity();
	float AspectRatio = 1024.f / 768.f;
	float m_fov = DirectX::XM_1DIV2PI / 2.0f;
};

}

#include "camera.h"
#include "legacy_render/render_subsystem.h"
#include "entity.h"

namespace luna
{

luna::LMatrix4f &CameraComponent::GetViewMatrix()
{
	m_view_matrix = m_cache_transform->GetMatrix().inverse();
	return m_view_matrix;
}

luna::LMatrix4f &CameraComponent::GetProjectionMatrix()
{
	float    SinFov;
	float    CosFov;
	SinFov = (float)sinf(0.5f * m_fov);
	CosFov = (float)cosf(0.5f * m_fov);

	float Height = CosFov / SinFov;
	float Width = Height / m_aspect_ratio;
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

void CameraComponent::OnCreate()
{
	m_need_tick = true;
	m_cache_transform = GetEntity()->AddComponent<Transform>();
}

void CameraComponent::OnTick(float delta_time)
{
	
}

}
#pragma once

#include "core/core_module.h"
#include "transform.h"
#include "world/component.h"
#include "legacy_render/interface/i_camera.h"

namespace luna
{

class WORLD_API CameraComponent : public Component, public ICamera
{
public:
	void OnCreate() override;
	void OnTick(float delta_time) override;


	LMatrix4f &GetViewMatrix() override;
	LMatrix4f &GetProjectionMatrix() override;

	GET_SET_VALUE(Transform *, m_cache_transform, Transform);
	GET_SET_VALUE(LVector3f, m_fly_direction, FlyDirection);
	GET_SET_VALUE(float, m_speed, Speed);

private:
	Transform *m_cache_transform;

	float zFar = 1000.f, zNear = 0.1f;
	LMatrix4f m_view_matrix = LMatrix4f::Identity();
	LMatrix4f m_proj_matrix = LMatrix4f::Identity();

	float m_aspect_ratio = 1024.f / 768.f;
	float m_fov = (float)M_PI / 3.0f;
	LVector3f m_fly_direction = LVector3f(0,0,0);
	float m_speed = 0.0f;
};

}
#include "camera.h"

namespace luna
{

void CameraComponent::SetAsMainCamera()
{

}

void CameraComponent::OnCreate()
{
	m_need_tick = true;
}

void CameraComponent::OnTick(float delta_time)
{
	
}

}
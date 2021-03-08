#pragma once

#include "core/core_module.h"
#include "core/subsystem/sub_system.h"
#include "scene.h"
#include "private_world.h"

namespace luna
{

class CameraComponent;

class WORLD_API WorldSubsystem : public SubSystem
{
public:
	WorldSubsystem() {};
	bool OnPreInit() override;

	bool OnPostInit() override;

	bool OnInit() override;

	bool OnShutdown() override;

	void Tick(float delta_time) override;

	GET_SET_VALUE(CameraComponent *, m_main_camera, MainCameraComponent);

private:
	CameraComponent *m_main_camera;
	SceneManager *manager;
};

}

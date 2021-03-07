#pragma once

#include "core/core_module.h"
#include "core/subsystem/sub_system.h"
#include "private_world.h"

namespace luna
{

class WORLD_API WorldSubsystem : public SubSystem
{
public:
	WorldSubsystem() {};
	bool OnPreInit() override;

	bool OnPostInit() override;

	bool OnInit() override;

	bool OnShutdown() override;

	void Tick(float delta_time) override;
private:
	SceneManager *manager;
};

}

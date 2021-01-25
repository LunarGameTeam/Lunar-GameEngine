#pragma once

#include "core/core_module.h"
#include "core/subsystem/sub_system.h"

namespace luna
{

class WorldSubsystem : public SubSystem
{
public:
	bool OnPreInit() override;

	bool OnPostInit() override;

	bool OnInit() override;

	bool OnShutdown() override;

	void Tick(float delta_time) override;

};

}

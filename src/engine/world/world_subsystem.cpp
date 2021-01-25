#include "world_subsystem.h"


namespace luna
{

bool luna::WorldSubsystem::OnPreInit()
{
	return true;
}

bool luna::WorldSubsystem::OnPostInit()
{
	return true;
}

bool luna::WorldSubsystem::OnInit()
{
	m_need_tick = true;
	return true;
}

bool luna::WorldSubsystem::OnShutdown()
{
	return true;
}

void luna::WorldSubsystem::Tick(float delta_time)
{

}

}
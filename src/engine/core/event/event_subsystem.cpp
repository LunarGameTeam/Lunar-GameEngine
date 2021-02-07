#include "event_subsystem.h"
#include "boost/make_shared.hpp"
#include "core/core_module.h"

namespace luna
{


bool EventSubsystem::OnPreInit()
{
	//("[Window]MainWindow Created");
	
	return true;
}

bool EventSubsystem::OnInit()
{
	m_need_tick = true;
	return true;

}

bool EventSubsystem::OnPostInit()
{
	return true;
}

bool EventSubsystem::OnShutdown()
{
	return true;
}

void EventSubsystem::Tick(float delta_time)
{

}

void EventSubsystem::Input(LWindow &window, InputEvent &event)
{
	OnInput.BroadCast(window, event);
}

}
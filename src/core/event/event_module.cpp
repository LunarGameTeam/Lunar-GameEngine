#include "event_module.h"
#include "core/framework/module.h"

namespace luna
{


RegisterTypeEmbedd_Imp(EventModule)
{

}


CORE_API EventModule *sEventModule = nullptr;

EventModule::EventModule()
{
	assert(sEventModule == nullptr);
	sEventModule = this;
}

bool EventModule::OnLoad()
{
	//("[Window]MainWindow Created");

	return true;
}

bool EventModule::OnInit()
{
	mNeedTick = true;
	return true;
}

bool EventModule::OnShutdown()
{
	return true;
}

void EventModule::Tick(float delta_time)
{
}

void EventModule::Input(LWindow &window, InputEvent &event)
{
	OnInput.BroadCast(window, event);
}

}
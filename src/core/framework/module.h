#pragma once


#include "core/object/base_object.h"
#include "core/misc/container.h"
#include "core/misc/string.h"
#include "core/misc/misc.h"
#include "core/misc/signal.h"

namespace luna
{

class CORE_API LModule : public LObject
{
	RegisterTypeEmbedd(LModule, LObject)
public:
	bool IsInitialized();
	bool Shutdown();

	virtual bool OnLoad() { return true; };	
	virtual bool OnInit() { return true; };
	virtual bool OnShutdown() { return true; };
	virtual void Tick(float delta_time);

	virtual void OnIMGUI();
	virtual void OnFrame(float delta_time);
	virtual void OnFrameBegin(float delta_time);
	virtual void OnFrameEnd(float delta_time);

	virtual const char *GetName();
protected:
	LModule() {};	
	bool mIsInitialized = false;
	bool mNeedTick = false;

	friend class LunaCore;
};

class LObject;


}

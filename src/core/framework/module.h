#pragma once


#include "core/object/base_object.h"
#include "core/foundation/container.h"
#include "core/foundation/string.h"
#include "core/foundation/misc.h"
#include "core/foundation/signal.h"

namespace luna
{

class CORE_API LModule : public LObject
{
	RegisterTypeEmbedd(LModule, LObject)
public:
	LModule() {};
	LModule(const LModule&) = delete;
	LModule(LModule&&) = delete;

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
	bool mIsInitialized = false;
	bool mNeedTick = false;

	friend class LunaCore;
};

class LObject;


}

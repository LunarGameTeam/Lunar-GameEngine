#pragma once


#include "Core/Object/BaseObject.h"
#include "Core/Foundation/Container.h"
#include "Core/Foundation/String.h"
#include "Core/Foundation/Misc.h"
#include "Core/Foundation/Signal.h"

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
	virtual void RenderTick(float delta_time);

	virtual void OnIMGUI();
	virtual void OnFrame(float delta_time);
	virtual void OnFrameBegin(float delta_time);
	virtual void OnFrameEnd(float delta_time);

	virtual const char *GetName();
protected:
	bool mIsInitialized = false;
	bool mNeedTick = false;
	bool mNeedRenderTick = false;

	friend class LunaCore;
};

class LObject;


}

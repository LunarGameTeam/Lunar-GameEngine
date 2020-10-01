#pragma once

#include "core/private_core.h"
#include "core/object/object.h"


#define DEPENDENCY_SUBSYSTEM_DECLARE(SubSystemClass,SystemVarName) \
	private:\
	ptr<SubSystemClass> SystemVarName;

#define DEPENDENCY_SUBSYSTEM_IMP(SubSystemClass,SystemVarName) \
	SystemVarName = gEngine->GetSubsystem<SubSystemClass>();

class SubSystem : public Object
{
public:
	bool Init();
	bool PreInit();
	bool PostInit();
	bool Shutdown();

	virtual bool OnPreInit() = 0;
	virtual bool OnPostInit() = 0;
	virtual bool OnInit() = 0;
	virtual bool OnShutdown() = 0;

	virtual void Tick() = 0;

public:
	bool mNeedTick = false;
};



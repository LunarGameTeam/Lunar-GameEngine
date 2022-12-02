#pragma once
#include "core/memory/ptr.h"
#include "core/misc/container.h"

namespace luna
{
class LObject;

class CORE_API LGcCore
{
	LUnorderedSet<LObject *> m_objects;
private:
	LGcCore() {};

public:
	inline static LGcCore *Instance()
	{
		static LGcCore* ins = new LGcCore();
		return ins;
	}

	//托管Object
	void HostObject(LObject *obj)
	{
		m_objects.insert(obj);
	}

	//后续扩展，分配内存，托管String分配等
	//托管String
	void HostString()
	{

	}
	//托管小内存物体
	void HostSmallObj()
	{

	}
};



}

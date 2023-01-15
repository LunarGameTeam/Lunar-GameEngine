#pragma once
#include "core/foundation/container.h"

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

};



}

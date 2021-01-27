#pragma once

#include "core/core_module.h"

namespace luna
{

class Component : public LObject
{
public:
	virtual void OnCreate();
	virtual void OnDestroy();
	void Destroy();

private:
	bool m_is_initliazed = false;
};

}
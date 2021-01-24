#pragma once

#include "core/core_module.h"
#include "component.h"

namespace luna
{

class Entity : LObject
{
public:
	template<typename T>
	T *AddComponent();
};

}
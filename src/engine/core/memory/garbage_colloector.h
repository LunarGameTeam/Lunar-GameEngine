#pragma once

#include "core/misc/container.h"
#include "core/object/object.h"

class Object;

namespace LunarEngine
{

class GrabageColloector
{
public:
	GrabageColloector &instance() {
		static GrabageColloector s_instance;
		return s_instance;
	}


	void Tick();
	

	static UnorderedSet<Object*> s_root_objects;

};

}
#include"core/object/object.h"
#include"core/memory/ptr.h"
class LResourceController
{
	LUnorderedMap<boost::uuids::uuid, LObject*> m_all_object_member;
private:
	LResourceController() {};
public:
	static LResourceController* GetInstance()
	{
		static LResourceController* this_instance;
		if (this_instance == NULL)
		{
			this_instance = new LResourceController();
		}
		return this_instance;
	}
	template<typename ObjectType, typename... Args>
	ObjectType* CreateObject(Args&&... arg);
};
template<typename ObjectType, typename... Args>
ObjectType* LResourceController::CreateObject(Args&&... arg)
{
	LunarEngine::LResult check_error;
	ObjectType* new_object = new ObjectType(arg...);
	check_error = new_object->InitResource();
	if (!check_error.m_IsOK)
	{
		delete new_object;
		return nullptr;
	}
	m_all_object_member.insert(std::pair<boost::uuids::uuid, LObject*>(new_object->GetLUuid().Get(), new_object));
	return new_object;
}
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









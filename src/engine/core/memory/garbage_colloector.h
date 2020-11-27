#pragma once
#include"core/object/object.h"
#include"core/memory/ptr.h"
#include "core/misc/container.h"

namespace LunarEngine
{
	class GrabageColloector
	{
		LUnorderedMap<boost::uuids::uuid, LObject*> m_all_object_member;
	private:
		GrabageColloector() {};
	public:
		static GrabageColloector* GetInstance()
		{
			static GrabageColloector* this_instance;
			if (this_instance == NULL)
			{
				this_instance = new GrabageColloector();
			}
			return this_instance;
		}
		template<typename ObjectType, typename... Args>
		ObjectType* CreateObject(Args&&... arg);
		void Tick();
	};
	template<typename ObjectType, typename... Args>
	ObjectType* GrabageColloector::CreateObject(Args&&... arg)
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

}









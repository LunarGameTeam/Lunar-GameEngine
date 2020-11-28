#pragma once
#include"core/object/object.h"
#include"core/memory/ptr.h"
#include "core/misc/container.h"

namespace luna
{
	class GrabageColloector
	{
		LUnorderedSet<LSharedObject*> m_all_object_member;
	private:
		GrabageColloector() {};
	public:
		static GrabageColloector* GetInstance()
		{
			static GrabageColloector this_instance;
			return &this_instance;
		}
		template<typename ObjectType, typename... Args>
		ObjectType* CreateObject(Args&&... arg);
		void Tick();
	};
	template<typename ObjectType, typename... Args>
	ObjectType* GrabageColloector::CreateObject(Args&&... arg)
	{
		luna::LResult check_error;
		ObjectType* new_object = new ObjectType(arg...);
		m_all_object_member.insert(new_object);
		return new_object;
	}

}









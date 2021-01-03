#pragma once
#include"core/object/object.h"
#include"core/memory/ptr.h"
#include "core/misc/container.h"

namespace luna
{
	class GrabageColloector
	{
		LUnorderedSet<LSharedObject*> m_all_object_member;
		LUnorderedMap<LString, LSharedObject*> m_named_object_member;
		LUnorderedMap<LSharedObject* ,LString> m_named_object_name;
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
		template<typename ObjectType, typename... Args>
		ObjectType* CreateObjectWithName(LString object_name, Args&&... arg);
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
	template<typename ObjectType, typename... Args>
	ObjectType* GrabageColloector::CreateObjectWithName(LString object_name, Args&&... arg)
	{
		luna::LResult check_error;
		if (m_named_object_member.find(object_name) != m_named_object_member.end())
		{
			return dynamic_cast<ObjectType*>(m_named_object_member[object_name]);
		}
		ObjectType* new_object = CreateObject<ObjectType>(arg...);
		m_named_object_member[object_name] = new_object;
		m_named_object_name[new_object] = object_name;
		return new_object;
	}
	//匿名object
	template<typename ObjectType, typename... Args>
	ObjectType* LCreateObject(Args&&... arg)
	{
		ObjectType* object_data = GrabageColloector::GetInstance()->CreateObject<ObjectType>(arg...);
		return object_data;
	}
	//有名object
	template<typename ObjectType, typename... Args>
	ObjectType* LCreateObjectWithName(LString object_name, Args&&... arg)
	{
		ObjectType* object_data = GrabageColloector::GetInstance()->CreateObjectWithName<ObjectType>(object_name,arg...);
		return object_data;
	}
}









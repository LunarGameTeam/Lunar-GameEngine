#include "object.h"
#include "core/memory/ptr.h"
using namespace boost;
using namespace LunarEngine;
void LSharedObject::AddSubObject(LSharedObject* sub_object)
{
	sub_object_list.push_back(sub_object);
}
const LLoadState& LObject::GetLoadState()
{
	CheckIfLoadingStateChanged(m_object_load_state);
	return m_object_load_state;
}
//默认加载资源
LunarEngine::LResult LObject::InitResource()
{
	LResult check_error;
	m_object_load_state = LLoadState::LOAD_STATE_EMPTY;
	check_error = InitCommon();
	if (!check_error.m_IsOK)
	{
		return check_error;
	}
	return g_Succeed;
}


LObject::LObject() :
	m_uid(LUuid::GenerateUUID())
{

}
LObject::~LObject()
{
}

//REGISTER_CLASS_IMP(Object)
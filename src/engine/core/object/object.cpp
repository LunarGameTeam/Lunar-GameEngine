#include "object.h"
#include "core/memory/ptr.h"
#include "core/core_configs.h"

using namespace boost;
using namespace luna;
LSharedObject*& LSharedObject::AddSubObject()
{
	return sub_object_list.emplace_back();
}
const LLoadState& LObject::GetLoadState()
{
	CheckIfLoadingStateChanged(m_object_load_state);
	return m_object_load_state;
}
//默认加载资源
luna::LResult LObject::InitResource()
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


luna::LResult LObject::InitCommon()
{
	return luna::g_Succeed;
}

LObject::LObject() :
	m_uid(GenerateUUID())
{

}
LObject::~LObject()
{
}

//REGISTER_CLASS_IMP(LObject)
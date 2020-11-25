#include "ptr.h"
#include "core/object/object.h"
LPtr::LPtr(LSharedObject* val)
{
	m_res_pointer = new LSharedObject*();
	*m_res_pointer = val;
	val->AddSubObject(*m_res_pointer);
}
LPtr::LPtr(const LPtr& copy)
{
	*m_res_pointer = *copy.m_res_pointer;
}
LPtr::~LPtr()
{
	if (m_res_pointer != nullptr)
	{
		*m_res_pointer = nullptr;
	}
};
LPtr& LPtr::operator=(const LPtr& val)
{
	*m_res_pointer = *val.m_res_pointer;
	return *this;
}
LPtr& LPtr::operator=(LSharedObject* val)
{
	*m_res_pointer = val;
	val->AddSubObject(*m_res_pointer);
}

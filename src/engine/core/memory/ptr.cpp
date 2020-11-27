#include "ptr.h"
#include "core/object/object.h"
LPtrBasic::LPtrBasic(LSharedObject* Parent)
{
	parent_pointer = Parent;
	m_res_pointer = &Parent->AddSubObject();
	*m_res_pointer = nullptr;
}
LPtrBasic::~LPtrBasic()
{
	if (m_res_pointer != nullptr)
	{
		*m_res_pointer = nullptr;
	}
};
void LPtrBasic::operator=(LSharedObject* val)
{
	SetValueToData(val);
}
void LPtrBasic::SetValueToData(LSharedObject* val)
{
	*m_res_pointer = val;
}

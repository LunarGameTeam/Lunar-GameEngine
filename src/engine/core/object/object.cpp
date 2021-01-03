#include "object.h"
#include "core/memory/ptr.h"
#include "core/core_configs.h"

using namespace boost;
using namespace luna;
LSharedObject*& LSharedObject::AddSubObject()
{
	return sub_object_list.emplace_back();
}

LObject::LObject() :
	m_uid(GenerateUUID())
{

}
LObject::~LObject()
{
}
//REGISTER_CLASS_IMP(LObject)
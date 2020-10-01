#include "object.h"

using namespace boost;
UnorderedSet<Ptr<Object>> Object::s_root_objects;

Object::Object() :
	m_uid(LUuid::GenerateUUID())
{

}

Object::~Object()
{
}

//REGISTER_CLASS_IMP(Object)
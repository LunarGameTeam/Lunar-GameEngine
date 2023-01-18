#include "Core/Memory/Ptr.h"

#include "Core/Object/BaseObject.h"
#include "Core/Memory/PtrBinding.h"
#include "Core/Reflection/Type.h"

namespace luna
{

PPtr::PPtr(LObject *parent) :
	mParent(parent)
{
}

PPtr::~PPtr()
{
	mInstanceID = 0;
};

LObject* PPtr::Get()
{
#ifdef _DEBUG
	if (mObject)
		return mObject;
	mObject = LObject::InstanceIDToObject(mInstanceID);
	return mObject;
#else
	return LObject::InstanceIDToObject(mInstanceID);
#endif // _DEBUG
}

void PPtr::SetPtr(LObject *val)
{
	if (val)
		mInstanceID = val->GetInstanceID();
	else
		mInstanceID = 0;
}

PPtr& PPtr::operator=(LObject *val)
{
	SetPtr(val);
	return *this;
}

}
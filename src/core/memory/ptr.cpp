#include "ptr.h"
#include "core/object/base_object.h"
#include "core/reflection/reflection.h"
#include "core/reflection/type.h"

namespace luna
{

LObject** s_NullPtr = nullptr;

LSubPtr::LSubPtr(LObject *parent) :
	mParent(parent),
	mPPtr(&parent->AllocateSubSlot())
{
}

LSubPtr::~LSubPtr()
{
	if (mPPtr != nullptr)
		mPPtr = nullptr;
};

void LSubPtr::SetPtr(LObject *val)
{
	*mPPtr = val;
}


void LSubPtr::operator=(LObject *val)
{
	SetPtr(val);
}

}
#include "ptr.h"

#include "core/object/base_object.h"
#include "core/memory/ptr_binding.h"
#include "core/reflection/type.h"

namespace luna
{

LObject** s_NullPtr = nullptr;

LSubPtr::LSubPtr(LObject *parent) :
	mParent(parent)
{
}

LSubPtr::~LSubPtr()
{
	if (mPPtr != nullptr)
		mPPtr = nullptr;
};

void LSubPtr::SetPtr(LObject *val)
{
	mPPtr = val;
}


LSubPtr& LSubPtr::operator=(LObject *val)
{
	SetPtr(val);
	return *this;
}

}
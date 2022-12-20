#include "base_object.h"
#include "core/memory/ptr.h"

#include "core/reflection/reflection.h"

#include "core/binding/binding_module.h"

using namespace boost;
using namespace luna;

size_t binding:: BindingLObject::sBindingObjectNum = 0;

RegisterTypeEmbedd_Imp(LObject)
{
	cls->Ctor<LObject>();	
	cls->Binding<LObject>();
	cls->BindingProperty<&LObject::mName>("name");

	BindingModule::Get("luna")->AddType(cls);
};

LObject*& LObject::AllocateSubSlot()
{
	mSubObjects.emplace_back(nullptr);
	return mSubObjects.back();
}

LObject::LObject():
	mHandle( new WeakPtrHandle(this)),
	mSelfHandle(this)
{

}

LObject::~LObject()
{
	Py_XDECREF(mBindingObject);
	mHandle->ptr = nullptr;	
}

void LObject::DeAllocateSubSlot(LObject **obj)
{
	if (obj)
	{
		for (auto it = mSubObjects.begin(); it != mSubObjects.end(); it++)
		{
			auto& ref = *it;

			if (ref == *obj && &ref == obj)
			{
				mSubObjects.erase(it);
				break;
			}			
		}
	}

}

LObject *LObject::GetParent()
{
	return mParent;
}

void LObject::Serialize(ISerializer &serializer)
{
	serializer.Serialize(this);
}

void LObject::DeSerialize(ISerializer &serializer)
{
	serializer.DeSerialize(this);
}

void LObject::SetBindingObject(PyObject* val)
{
	assert(mBindingObject == nullptr);
	Py_XINCREF(val);
	mBindingObject = (binding::BindingLObject*)(val);
	memset(&mBindingObject->ptr, 0, sizeof(mBindingObject->ptr));
	mBindingObject->ptr = this;	
}
#include "base_object.h"

#include "core/reflection/type.h"
#include "core/reflection/reflection.h"

#include "core/binding/binding_module.h"
#include "core/memory/ptr.h"

#include <unordered_map>



namespace luna
{

size_t binding::BindingLObject::sBindingObjectNum = 0;
size_t sInstanceID = 0;
std::unordered_map<size_t, LObject*> sObjects;


PyObject* GetPropertiese(PyObject* cls, PyObject* args)
{
	LType* type = LType::Get((PyTypeObject*)cls);
	std::vector<LProperty*> props;
	if (type)
	{
		type->GetAllProperties(props);
	}
	return to_binding(props);
}

RegisterTypeEmbedd_Imp(LObject)
{
	cls->Ctor<LObject>();
	cls->Binding<LObject>();
	cls->BindingMethod<&GetPropertiese, MethodType::ClassFunction>("get_properties");
	cls->BindingProperty<&LObject::mName>("name");

	BindingModule::Get("luna")->AddType(cls);
};

LObject*& LObject::AllocateSubSlot()
{
	mSubObjects.emplace_back(nullptr);
	return mSubObjects.back();
}

LObject::LObject() :
	mInstanceID(sInstanceID++)
{
	sObjects[mInstanceID] = this;
}

LObject::~LObject()
{
	Py_XDECREF(mBindingObject);
}

void LObject::DeAllocateSubSlot(LObject** obj)
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

LObject* LObject::GetParent()
{
	return mParent;
}

void LObject::Serialize(ISerializer& serializer)
{
	serializer.Serialize(this);
}

void LObject::DeSerialize(ISerializer& serializer)
{
	serializer.DeSerialize(this);
}

LObject* LObject::InstanceIDToObject(size_t id)
{
	return sObjects[id];
}

void LObject::SetBindingObject(PyObject* val)
{
	assert(mBindingObject == nullptr);
	Py_XINCREF(val);
	mBindingObject = (binding::BindingLObject*)(val);
	memset(&mBindingObject->ptr, 0, sizeof(mBindingObject->ptr));
	mBindingObject->ptr = this;
}
}
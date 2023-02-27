#include "Core/Object/BaseObject.h"

#include "Core/Reflection/Type.h"
#include "Core/Reflection/Reflection.h"

#include "Core/Serialization/JsonSerializer.h"
#include "Core/Scripting/BindingModule.h"
#include "Core/Memory/Ptr.h"

#include <unordered_map>



namespace luna
{

size_t binding::BindingLObject::sBindingObjectNum = 0;
size_t sInstanceID = 1;
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

LObject::LObject() :
	mInstanceID(sInstanceID++)
{
	sObjects[mInstanceID] = this;
}

LObject::~LObject()
{
	auto tmp = mSubObjects;
	for (LObject* it : tmp)
	{
		delete it;
	}
	if (mParent)
	{
		mParent->mSubObjects.remove(this);
		mParent = nullptr;
	}
	mSubObjects.clear();
	sObjects.erase(mInstanceID);
	Py_XDECREF(mBindingObject);
}

void LObject::SetParent(LObject* parent)
{
	if (mParent != nullptr)
		return;
	if (mParent == parent)
		return;
	mParent = parent;
	if(parent)
		parent->mSubObjects.push_back(this);
}

void LObject::ForEachSubObject(std::function<void(size_t, LObject*)> func)
{
	size_t idx = 0;
	for (LObject* it : mSubObjects)
	{
		func(idx, it);
	}
}

size_t LObject::Index() const
{
	size_t idx = 0;
	if (mParent)
	{
		
		for (LObject* obj : mParent->mSubObjects)
		{
			if (obj == this)
				break;				
			idx++;
		}
		
	}
	return idx;
}


LObject* LObject::GetParent()
{
	return mParent;
}

void LObject::Serialize(Serializer& serializer)
{
	serializer.Serialize(this);
}

void LObject::DeSerialize(Serializer& serializer)
{
	serializer.DeSerialize(this);
}

LObject* LObject::InstanceIDToObject(size_t id)
{
	return sObjects[id];;
}

void LObject::SetBindingObject(PyObject* val)
{
	assert(mBindingObject == nullptr);
	Py_XINCREF(val);
	mBindingObject = static_cast<binding::BindingObject*>(val);
	binding::BindingLObject* obj = static_cast<binding::BindingLObject*>(val);
	memset(&obj->ptr, 0, sizeof(obj->ptr));
	obj->ptr = this;
}
}

namespace luna::binding
{

PyObject* BindingLObject::__alloc__(PyTypeObject* type, Py_ssize_t size)
{
	BindingLObject* bindingObject = (BindingLObject*)PyType_GenericAlloc(type, size);
	return bindingObject;
}

void BindingLObject::__destrctor__(PyObject* self)
{
	PyTypeObject* tp = Py_TYPE(self);
	binding::BindingLObject* obj = (binding::BindingLObject*)(self);
	obj->~BindingLObject();
	tp->tp_free(self);
}

int BindingLObject::__bool__(PyObject* self)
{
	binding::BindingLObject* obj = (binding::BindingLObject*)(self);
	if (obj->ptr.Get())
		return 1;
	return 0;
}


PyObject* BindingLObject::__new__(PyTypeObject* pytype, PyObject* args, PyObject* kwrds)
{
	Py_XINCREF(pytype);
	LType* type = LType::Get(pytype);
	binding::BindingLObject* obj = (binding::BindingLObject*)pytype->tp_alloc(pytype, 0);	
	LObject* t = type->NewInstance<LObject>();
	t->SetType(type);
	t->SetBindingObject(obj);
	return (PyObject*)obj;
}

}
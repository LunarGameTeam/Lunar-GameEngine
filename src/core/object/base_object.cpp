#include "base_object.h"

#include "core/reflection/type.h"
#include "core/reflection/reflection.h"

#include "core/serialization/serialization.h"
#include "core/binding/binding_module.h"
#include "core/memory/ptr.h"

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
	if (mParent)
	{
		mParent->mSubObjects.remove(this);
		mParent = nullptr;
	}
	auto tmp = mSubObjects;
	for (LObject* it : tmp)
	{
		delete it;
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

namespace luna::binding
{

PyObject* BindingLObject::__alloc__(PyTypeObject* type, Py_ssize_t size)
{
	BindingLObject* bindingObject = (BindingLObject*)PyType_GenericAlloc(type, size);
	std::construct_at<PPtr>(&(bindingObject->ptr));
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

PyObject* BindingLObject::__new__(PyTypeObject* type, PyObject* args, PyObject* kwrds)
{
	Py_XINCREF(type);
	LType* object_type = LType::Get(type);
	binding::BindingLObject* obj = (binding::BindingLObject*)type->tp_alloc(type, 0);
	LObject* t = object_type->NewInstance<LObject>();

#ifdef _DEBUG
	PyObject* dict = PyObject_GenericGetDict(obj, nullptr);
	//CheckÒ»ÏÂDict
	LUNA_ASSERT(PyDict_Check(dict));
	Py_XDECREF(dict);
#endif

	t->SetType(object_type);
	t->SetBindingObject(obj);
	return (PyObject*)obj;
}

}
#include "Core/Object/SharedObject.h"

namespace luna
{

RegisterTypeEmbedd_Imp(SharedObject)
{
	cls->Ctor<SharedObject>();
	cls->Binding<SharedObject>();
	BindingModule::Get("luna")->AddType(cls);
}

void SharedObject::SetBindingObject(PyObject* val)
{
	auto obj = static_cast<binding::BindingSharedObject*>(val);
	obj->ptr = ToSharedPtr<SharedObject>(this);
};

namespace binding
{

PyObject* BindingSharedObject::__alloc__(PyTypeObject* type, Py_ssize_t size)
{
	auto bindingObject = static_cast<BindingSharedObject*>(PyType_GenericAlloc(type, size));
	std::construct_at<std::shared_ptr<SharedObject>>(&(bindingObject->ptr));
	return bindingObject;
}

void BindingSharedObject::__destrctor__(PyObject* self)
{
	PyTypeObject* tp = Py_TYPE(self);
	auto obj = static_cast<binding::BindingSharedObject*>(self);
	obj->~BindingSharedObject();
	tp->tp_free(self);
}

int BindingSharedObject::__bool__(PyObject* self)
{
	BindingSharedObject* obj = static_cast<binding::BindingSharedObject*>(self);	
	return obj->ptr ? 1 : 0;
	
}


PyObject* BindingSharedObject::__new__(PyTypeObject* pytype, PyObject* args, PyObject* kwrds)
{
	Py_XINCREF(pytype);
	LType* type = LType::Get(pytype);
	BindingSharedObject* obj = static_cast<BindingSharedObject*>(pytype->tp_alloc(pytype, 0));
	auto t = type->NewInstance<LObject>();
	t->SetType(type);
	t->SetBindingObject(obj);
	return (PyObject*)obj;
}

}
}
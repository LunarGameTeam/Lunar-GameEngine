#pragma once 

#include "binding_python.h"

namespace luna::binding
{

struct binding_proxy_base
{
	using BindingType = void;

	void* GetNativePtr() { return nullptr; }

	static int get_type_flags() { return Py_TPFLAGS_DEFAULT; };
	static setattrofunc get_setattrofunc() { return nullptr; };
	static getattrofunc get_getattrofunc() { return nullptr; };
	static initproc get_initproc() { return nullptr; }
	static destructor get_destructor() { return nullptr; }
	static newfunc get_newfunc() { return nullptr; }

};

template<typename T, class = void>
struct binding_proxy : binding_proxy_base
{
	using BindingType = typename T;
};

template<typename T>
struct struct_binding_proxy : binding_proxy_base
{
	static int get_type_flags() { return Py_TPFLAGS_DEFAULT; };

	using BindingType = BindingStruct<T>;

	static newfunc get_newfunc() { return (newfunc)PyType_GenericNew; }

	static BindingStruct<T>* new_binding_object()
	{
		PyTypeObject* type = binding_type<T>::StaticType();
		return PyObject_NEW(BindingStruct<T>, type);
	}

	template<typename M>
	static PyObject* raw_getter(BindingStruct<T>* o, void* closure)
	{
		BindingProperty& prop = *(BindingProperty*)(closure);
		size_t offset = prop.mOffset;
		M* mem_ptr = (M*)((char*)o->GetPtr() + offset);
		M& mem = *mem_ptr;
		return binding_converter<M>::to_binding(mem);
	}
	template<typename M>
	static int raw_setter(BindingNative<T>* o, PyObject* val, void* closure)
	{
		BindingProperty& prop = *(BindingProperty*)(closure);
		size_t offset = prop.mOffset;
		M* mem_ptr = (M*)((char*)o->GetPtr() + offset);
		*mem_ptr = binding_converter<M>::from_binding(val);
		return 0;
	}
};

template<typename T>
struct native_binding_proxy : binding_proxy_base
{
	using BindingType = BindingNative<T>;

	template<typename M>
	static PyObject* raw_getter(PyObject* s, void* closure)
	{
		BindingNative<T>* o = (BindingNative<T>*)(s);
		BindingProperty& prop = *(BindingProperty*)(closure);
		size_t offset = prop.mOffset;
		M* mem_ptr = (M*)((char*)o->val + offset);
		M& mem = *mem_ptr;
		return binding_converter<M>::to_binding(mem);
	}

	template<typename M>
	static int raw_setter(PyObject* s, PyObject* val, void* closure)
	{
		BindingNative<T>* o = (BindingNative<T>*)(s);
		BindingProperty& prop = *(BindingProperty*)(closure);
		size_t offset = prop.mOffset;
		M* mem_ptr = (M*)((char*)o->val + offset);
		*mem_ptr = binding_converter<M>::from_binding(val);
		return 0;
	}

};


}
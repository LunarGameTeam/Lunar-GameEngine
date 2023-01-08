#pragma once 

#include "binding_python.h"

namespace luna::binding
{

struct binding_proxy_base
{
	using binding_object_t = void;

	static constexpr setattrofunc get_setattro = nullptr;
	static constexpr initproc get_initproc = nullptr;
	static constexpr inquiry get_bool = nullptr;
	static constexpr destructor get_destructor = nullptr;
	static constexpr getattrofunc get_getattrofunc = nullptr;
	static constexpr newfunc get_newfunc = nullptr;
	static constexpr allocfunc get_allocfunc = nullptr;
	static constexpr binaryfunc get_multiply = nullptr;
	static constexpr binaryfunc get_add = nullptr;
	static constexpr richcmpfunc get_richcmpfunc = nullptr;

	void* GetNativePtr() { return nullptr; }

	static int get_type_flags() { return Py_TPFLAGS_DEFAULT; };
};

template<typename T, class = void>
struct binding_proxy : binding_proxy_base
{
	using binding_object_t = BindingObject;
};

template<typename T>
struct struct_binding_proxy : binding_proxy_base
{
	using binding_object_t = BindingStruct<T>;

	constexpr static newfunc get_newfunc = PyType_GenericNew;

	static int get_type_flags() { return Py_TPFLAGS_DEFAULT; };
		
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
	using binding_object_t = BindingNative<T>;

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
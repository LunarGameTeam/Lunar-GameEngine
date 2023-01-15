#pragma once

#include "core/math/math.h"
#include "core/binding/binding.h"

namespace luna::binding
{


template<typename T>
static PyObject* __add__(PyObject* left, PyObject* right)
{
	BindingStruct<T>* l = (BindingStruct<T>*)left;
	BindingStruct<T>* r = (BindingStruct<T>*)right;	
	return to_binding<T>(l->val + r->val);
}

template<typename T>
static PyObject* __richcmp__(PyObject* left, PyObject* right, int op)
{
	BindingStruct<T>* l = (BindingStruct<T>*)left;
	if (!PyObject_TypeCheck(right, static_type<T>::StaticType()->GetBindingType()))
		Py_RETURN_FALSE;
	BindingStruct<T>* r = (BindingStruct<T>*)right;
	switch (op)
	{
	case Py_EQ:
		if (l->val == r->val)
			Py_RETURN_TRUE;
		else
			Py_RETURN_FALSE;
		break;
	case Py_NE:
		if (l->val != r->val)
			Py_RETURN_TRUE;
		else
			Py_RETURN_FALSE;
		break;
	default:
		break;
	}
	Py_RETURN_FALSE;
}


template<>
struct binding_proxy<LVector2f> : struct_binding_proxy<LVector2f>
{
	static int __initproc__(PyObject* s, PyObject* args, PyObject* kwrds)
	{
		BindingStruct<LVector2f>* self = (BindingStruct<LVector2f>*)s;
		Py_INCREF(args);
		Py_ssize_t size = PyTuple_GET_SIZE(args);
		float x[2] = { 0, 0};
		for (int i = 0; i < size; ++i)
		{
			PyObject* arg1 = PyTuple_GET_ITEM(args, i);
			Py_INCREF(arg1);
			double d = PyFloat_AsDouble(arg1);
			Py_DECREF(arg1);
			x[i] = float(d);
		}
		self->val = LVector2f(x[0], x[1]);
		Py_DECREF(args);
		return 1;
	}
	constexpr static initproc get_initproc = __initproc__;
	constexpr static binaryfunc get_add = __add__<LVector2f>;
	constexpr static richcmpfunc get_richcmpfunc = __richcmp__<LVector2f>;
};

template<>
struct binding_proxy<LVector3f> : struct_binding_proxy<LVector3f>
{
	static int __initproc__(PyObject* s, PyObject* args, PyObject* kwrds)
	{
		BindingStruct<LVector3f>* self = (BindingStruct<LVector3f>*)s;
		Py_INCREF(args);
		Py_ssize_t size = PyTuple_GET_SIZE(args);
		float x[3] = { 0, 0, 0};
		for (int i = 0; i < size; ++i)
		{
			PyObject* arg1 = PyTuple_GET_ITEM(args, i);
			Py_INCREF(arg1);
			double d = PyFloat_AsDouble(arg1);
			Py_DECREF(arg1);
			x[i] = float(d);
		}
		self->val = LVector3f(x[0], x[1], x[2]);
		Py_DECREF(args);
		return 1;
	}
	constexpr static initproc get_initproc = __initproc__;
	constexpr static binaryfunc get_add = __add__<LVector3f>;
	constexpr static richcmpfunc get_richcmpfunc = __richcmp__<LVector3f>;
};

template<>
struct binding_proxy<LVector4f> : struct_binding_proxy<LVector4f>
{
	static int __initproc__(PyObject* s, PyObject* args, PyObject* kwrds)
	{
		BindingStruct<LVector4f>* self = (BindingStruct<LVector4f>*)s;
		Py_INCREF(args);
		Py_ssize_t size = PyTuple_GET_SIZE(args);
		float x[4] = { 0, 0, 0, 0 };
		for (int i = 0; i < size; ++i)
		{
			PyObject* arg1 = PyTuple_GET_ITEM(args, i);
			Py_INCREF(arg1);
			double d = PyFloat_AsDouble(arg1);
			Py_DECREF(arg1);
			x[i] = float(d);
		}
		self->val = LVector4f(x[0], x[1], x[2], x[3]);
		Py_DECREF(args);
		return 1;
	}
	constexpr static initproc get_initproc = __initproc__;
	constexpr static binaryfunc get_add = __add__<LVector4f>;
	constexpr static richcmpfunc get_richcmpfunc = __richcmp__<LVector4f>;
};

template<>
struct binding_proxy<LQuaternion> : struct_binding_proxy<LQuaternion>
{
	
	static int __initproc__(PyObject* s, PyObject* args, PyObject* kwrds)
	{
		BindingStruct<LQuaternion>* self = (BindingStruct<LQuaternion>*)s;
		Py_INCREF(args);
		Py_ssize_t size = PyTuple_GET_SIZE(args);
		float x[4] = { 0, 0, 0, 0 };
		for (int i = 0; i < size; ++i)
		{
			PyObject* arg1 = PyTuple_GET_ITEM(args, i);
			Py_INCREF(arg1);
			double d = PyFloat_AsDouble(arg1);
			Py_DECREF(arg1);
			x[i] = float(d);
		}
		self->val = LQuaternion(x[0], x[1], x[2], x[3]);
		Py_DECREF(args);
		return 1;
	}

	static PyObject* __multiply__(PyObject* l, PyObject* r)
	{
		BindingStruct<LQuaternion>* left = (BindingStruct<LQuaternion>*)(l);
		if (CheckBindingType<LQuaternion>(r))
		{
			BindingStruct<LQuaternion>* right = (BindingStruct<LQuaternion>*)(r);
			return to_binding(left->val * right->val);
		}
		else if (CheckBindingType<LVector3f>(r))
		{
			BindingStruct<LVector3f>* right = (BindingStruct<LVector3f>*)(r);
			return to_binding(left->val * right->val);
		}
		Py_RETURN_NONE;
	}

	constexpr static binaryfunc get_multiply = __multiply__;
	constexpr static initproc get_initproc = __initproc__;
	constexpr static richcmpfunc get_richcmpfunc = __richcmp__<LQuaternion>;
};

template<>
struct binding_proxy<LMatrix4f> : struct_binding_proxy<LMatrix4f>
{
};

}
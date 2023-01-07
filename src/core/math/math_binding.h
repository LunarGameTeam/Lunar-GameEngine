#pragma once

#include "core/math/math.h"
#include "core/binding/binding.h"

namespace luna::binding
{

template<>
struct binding_proxy<LVector2f> : struct_binding_proxy<LVector2f>
{
	static initproc get_initproc() { return __initproc__; }
	static int __initproc__(PyObject* s, PyObject* args, PyObject* kwrds)
	{
		BindingStruct<LVector2f>* self = (BindingStruct<LVector2f>*)s;
		Py_INCREF(args);
		Py_ssize_t size = PyTuple_GET_SIZE(args);
		float x = 0, y = 0, z = 0;
		if (size > 0)
		{
			PyObject* arg1 = PyTuple_GET_ITEM(args, 0);
			Py_INCREF(arg1);
			double d = PyFloat_AsDouble(arg1);
			Py_DECREF(arg1);
			x = float(d);
		}
		if (size > 1)
		{
			PyObject* arg2 = PyTuple_GET_ITEM(args, 1);
			Py_INCREF(arg2);
			double d = PyFloat_AsDouble(arg2);
			Py_DECREF(arg2);
			y = float(d);
		}
		self->val = LVector2f(x, y);
		Py_DECREF(args);
		return 1;
	}
};

template<>
struct binding_proxy<LVector3f> : struct_binding_proxy<LVector3f>
{
	static initproc get_initproc() { return __initproc__; }
	static int __initproc__(PyObject* s, PyObject* args, PyObject* kwrds)
	{
		BindingStruct<LVector3f>* self = (BindingStruct<LVector3f>*)s;
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
		self->val = LVector3f(x[0], x[1], x[2]);
		Py_DECREF(args);
		return 1;
	}
};

template<>
struct binding_proxy<LVector4f> : struct_binding_proxy<LVector4f>
{
	static initproc get_initproc() { return __initproc__; }
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
};

template<>
struct binding_proxy<LQuaternion> : struct_binding_proxy<LQuaternion>
{
	static initproc get_initproc() { return __initproc__; }
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
};


}
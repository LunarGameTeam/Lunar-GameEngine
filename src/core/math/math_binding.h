#pragma once

#include "core/math/math.h"
#include "core/binding/binding.h"

namespace luna::binding
{

template<>
struct binding_proxy<LVector3f> : struct_binding_proxy<LVector3f>
{
	static initproc get_initproc() { return __initproc__; }
	static int __initproc__(PyObject* s, PyObject* args, PyObject* kwrds)
	{
		BindingStruct<LVector3f>* self = (BindingStruct<LVector3f>*)s;
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
		if (size > 2)
		{
			PyObject* arg3 = PyTuple_GET_ITEM(args, 2);
			Py_INCREF(arg3);
			double d = PyFloat_AsDouble(arg3);
			Py_DECREF(arg3);
			z = float(d);
		}
		self->val = LVector3f(x, y, z);
		Py_DECREF(args);
		return 1;
	}
};

}
#pragma once

#include "core/core_config.h"
#include <xmemory>

#define PY_SSIZE_T_CLEAN

#ifdef USING_DEBUG_PYTHON

#ifndef _DEBUG
#include "Python.h"
#undef _DEBUG
#else
#include "Python.h"
#endif // !_DEB


#else

#undef _DEBUG
#include "Python.h"
#define _DEBUG

#endif // USING_DEBUG_PYTHON


namespace luna::binding
{

struct CORE_API BindingObject : PyObject
{

};

//���ڴ洢C++�����PyObject������Python���ü���Ӱ��
template<typename T>
struct BindingNative : public BindingObject
{
	T* val;

	T* GetPtr() { return val; }
};

//���ڴ洢һ��Struct��PyObject��ֵ����
template<typename T>
struct BindingStruct : public BindingObject
{
	T val;

	T* GetPtr() { return &val; }
};


}
/*!
 * \file core.h
 * \date 2020/08/09 10:11
 *
 * \author isAk wOng
 *
 * \brief 提供给其他模块包含的头文件，会包含本模块的基本内容和接口
 *
 * TODO: long description
 *
 * \note
*/
#pragma once


#include "Core/CoreMin.h"

#include "Core/Foundation/Misc.h"
#include "Core/Foundation/Signal.h"
#include "Core/Foundation/String.h"

#include "Core/Memory/Ptr.h"
#include "Core/Memory/PtrBinding.h"
#include "Core/Foundation/Config.h"

#include "Core/CoreMin.h"
#include "windows.h"

namespace luna
{

CORE_API void LoadLib(const luna::LString& val)
{
	HINSTANCE hDll = LoadLibraryExA(val.c_str(), NULL, 0);
	auto e =GetLastError();
	assert(hDll != nullptr);
	return;
}

CORE_API void AddLibDir(const luna::LString& val)
{
	SetDefaultDllDirectories(LOAD_LIBRARY_SEARCH_DEFAULT_DIRS);
	AddDllDirectory(StringToWstring(val.std_str()).c_str());
	return;
}


CORE_API PyObject* Print(PyObject* self, PyObject* args)
{
	auto count = PyTuple_GET_SIZE(args);
	for (int i = 0; i < count; ++i)
	{
		PyObject* arg = PyTuple_GetItem(args, i);
		PyObject* objectsRepresentation = PyObject_Repr(arg);
		const char* msg = PyUnicode_AsUTF8(objectsRepresentation);
		Log("Python", msg);
		Py_XDECREF(objectsRepresentation);
	}
	Py_RETURN_NONE;
}
STATIC_INIT(Core)
{

	LType::Get<LVector2f>();
	LType::Get<LVector3f>();
	LType::Get<LVector4f>();
	LType::Get<LQuaternion>();

	LType::Get<PPtrArray>()->Binding<PPtrArray>();
	BindingModule::Luna()->AddType(LType::Get<PPtrArray>());
	BindingModule::Luna()->AddMethod<&LoadLib>("load_library");
	BindingModule::Luna()->AddMethod<&AddLibDir>("add_library_dir");	
	BindingModule::Luna()->AddCFunction<&Print>("print");
};

}
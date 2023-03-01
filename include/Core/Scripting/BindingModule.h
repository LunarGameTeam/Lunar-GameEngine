#pragma once


#include "Core/Scripting/Binding.h"
#include "Core/CoreTypes.h"
#include "Core/Foundation/String.h"
#include "Core/Foundation/Container.h"
#include <map>
#include <vector>

namespace luna
{

// Luna Engine 对 Python Module 的一个封装
// 延迟创建Module，等到Python完全初始化后才进行创建
// 可以提前塞入一些定义
class CORE_API BindingModule
{
protected:
	BindingModule(const LString& module_name);

public:

	PyObject* GetBinding() { return mPythonModule; };
	void AddType(LType* type);

	template<auto fn>
	PyMethodDef& AddMethod(const char* name)
	{
		using FN = decltype(fn);
		constexpr auto args_count = function_traits<FN>::args_count;
		PyCFunction p = binding::static_pycfunction_select<fn>(std::make_index_sequence<args_count>{});
		const LString& method_name = LString::MakeStatic(name);
		PyMethodDef& def = mMethods[method_name]; 		
		def.ml_name = method_name.c_str();
		def.ml_meth = p;
		def.ml_flags = METH_VARARGS;
		def.ml_doc = LString::MakeStatic(binding::static_method_doc<fn>(method_name.c_str()));
		return def;
	}

	template<auto lambda_v>
	PyMethodDef& AddLambda(const char* name)
	{

		using lambda_t = decltype(lambda_v);
		constexpr auto fn = &lambda_t::operator();
		constexpr auto args_count = function_traits<decltype(fn)>::args_count;
		PyCFunction p = binding::lambda_pycfunction_select<lambda_t, fn>(std::make_index_sequence<args_count>{});
		const LString& method_name = LString::MakeStatic(name);
		PyMethodDef& def = mMethods[method_name];
		def.ml_name = method_name.c_str();
		def.ml_meth = p;
		def.ml_flags = METH_VARARGS;
		def.ml_doc = LString::MakeStatic(binding::static_method_doc<fn>(method_name.c_str()));
		return def;
	}
	
	template<auto fn>
	PyMethodDef& AddCFunction(const char* name)
	{
		using FN = decltype(fn);
		const LString& method_name = LString::MakeStatic(name);
		PyMethodDef& def = mMethods[method_name];
		def.ml_name = method_name.c_str();
		def.ml_meth = fn;
		def.ml_flags = METH_VARARGS;
		def.ml_doc = LString::MakeStatic(binding::static_method_doc<fn>(method_name.c_str()));
		return def;
	}

	void AddObject(const char* name, PyObject* obj);	
	void AddLObject(const char* name, LObject* obj);
	void AddConstant(const char* name, const char* val);
	void AddConstant(const char* name, long val);	
	void AddSubModule(const char* name, BindingModule* module);

	bool Init();
	const LString& GetModuleName() { return mModuleName; }

	void GenerateDoc();

	static BindingModule* Get(const char* module_name);
	static BindingModule* Luna();

	

	static PyObject* meta_type_new(PyTypeObject* metatype, PyObject* args, PyObject* kwds);
private:

	void _AddType(PyTypeObject* type);
	void _AddObject(const char* name, PyObject* obj);

	std::vector<PyTypeObject*>        mInitTypes;
	bool                              mModuleInited = false; 	
	PyModuleDef                       mModuleDef;
	std::map<LString, PyObject*>      mConstants;
	std::map<LString, PyObject*> 	  mConstantStr;

	std::map<LString, PyTypeObject*>  mTypes;
	std::map<LString, PyObject*>      mObjects;
	std::map<LString, PyMethodDef>    mMethods;
	BindingModule*                    mParent       = nullptr;
	std::map<LString, BindingModule*> mSubModules;
	LString                           mModuleName;
	PyObject*                         mPythonModule = nullptr;

};

CORE_API extern LMap<LString, BindingModule*>* sBindingModules;


}
#pragma once


#include "core/binding/binding.h"
#include "core/core_types.h"
#include "core/foundation/string.h"
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

	std::vector<PyTypeObject*>        m_order_types;
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

CORE_API extern std::map<LString, BindingModule*>* sBindingModules;


}
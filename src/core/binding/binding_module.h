#pragma once


#include "core/binding/binding.h"
#include "core/core_types.h"
#include "core/foundation/string.h"
#include <map>
#include <vector>

namespace luna
{

class CORE_API LBindingModule
{
protected:
	LBindingModule(const LString& module_name);

public:

	PyObject* GetBinding() { return m_binding_module; };
	void AddType(LType* type);

	template<auto fn>
	PyMethodDef& AddMethod(const char* name)
	{
		using FN = decltype(fn);
		constexpr auto args_count = function_traits<FN>::args_count;
		PyCFunction p = binding::pycfunction_select<fn>(std::make_index_sequence<args_count>{});
		const LString& method_name = LString::MakeStatic(name);
		PyMethodDef& def = m_methods[method_name]; 		
		def.ml_name = method_name.c_str();
		def.ml_meth = p;
		def.ml_flags = METH_VARARGS;
		def.ml_doc = LString::MakeStatic(binding::static_method_doc<fn>(method_name.c_str())).c_str();
		return def;
	}

	void AddObject(const char* name, PyObject* obj);	
	void AddLObject(const char* name, LObject* obj);
	void AddConstant(const char* name, long val);

	bool Init();
	const LString& GetModuleName() { return m_module_name; }

	void GenerateDoc();
	static PyObject* meta_type_new(PyTypeObject* metatype, PyObject* args, PyObject* kwds);
public:
	bool m_inited = false;
	PyModuleDef m_module_def;
	static LBindingModule* Get(const char* module_name);
	static LBindingModule* Luna();
	std::map<LString, long> m_constants;
	std::map<LString, PyTypeObject*> m_types;
	std::vector<PyTypeObject*> m_order_types;
	std::map<LString, PyObject*> m_objects;
	std::map<LString, PyMethodDef> m_methods;
	std::vector<PyMethodDef> m_methods_data;
	LString m_module_name;
	PyObject* m_binding_module;

};

CORE_API extern std::map<LString, LBindingModule*>* g_modules;


}
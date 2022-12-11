#include "core/binding/binding_module.h"
#include "core/math/math.h"
#include "core/binding/binding_traits.h"
#include "core/foundation/container.h"
#include "core/object/base_object.h"
#include "core/framework/module.h"

namespace luna
{
std::map<LString, LBindingModule*>* g_modules;

void LBindingModule::GenerateDoc()
{
	LString doc= "";
	for (auto method : m_methods)
	{
		if(method.second.ml_doc)
			doc = doc + method.second.ml_doc + "\n\n";
	}
	m_module_def.m_doc = LString::MakeStatic(doc).c_str();

}

PyObject* LBindingModule::meta_type_new(PyTypeObject* metatype, PyObject* args, PyObject* kwds)
{
	PyTypeObject* binding_type = (PyTypeObject*)PyType_Type.tp_new(metatype, args, kwds);
	LType* base = LType::Get(binding_type->tp_base);
	LType* type = NewType(binding_type->tp_name, binding_type->tp_basicsize, LType::Get(binding_type->tp_base));
	type->SetBindingType(binding_type);
	type->mConstructable = base->mConstructable;
	type->mCtor = base->mCtor;
	type->mIsPureBindingType = true;
	return (PyObject*)binding_type;
}

PyObject* GetTypeProperties(PyObject* self, PyObject* args)
{
	LType* type = LType::Get((PyTypeObject*)self);	
	std::vector<LProperty*> props;
	type->GetAllProperties(props);	
	return to_binding(props);
}

void InitMetaType()
{
	sMetaType = (PyTypeObject*)PyType_GenericAlloc(&PyType_Type, 0);
	static std::vector<PyMethodDef> methods;
	auto* ht = (PyHeapTypeObject*)sMetaType;
	sMetaType->tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_HEAPTYPE | Py_TPFLAGS_BASETYPE;
	sMetaType->tp_base = &PyType_Type;
	ht->ht_name = PyUnicode_FromString("MetaType");
	Py_ssize_t t;
	sMetaType->tp_name = PyUnicode_AsUTF8AndSize(ht->ht_name, &t);
	sMetaType->tp_as_number = &ht->as_number;
	sMetaType->tp_as_sequence = &ht->as_sequence;
	sMetaType->tp_as_mapping = &ht->as_mapping;
	sMetaType->tp_as_buffer = &ht->as_buffer;
	sMetaType->tp_new = (newfunc)&LBindingModule::meta_type_new;	
	methods.emplace_back("get_properties",GetTypeProperties, METH_VARARGS, nullptr);
	methods.emplace_back(nullptr);
	sMetaType->tp_methods = methods.data();
	Py_INCREF(sMetaType);
	if (PyType_Ready(sMetaType) < 0)
	{
		assert(0);
	}
}


LBindingModule::LBindingModule(const LString& module_name)
{
	m_module_name = module_name;
	g_modules->operator [](module_name) = this;
}

void LBindingModule::AddType(LType* type)
{
	assert(type->mBindingModule == nullptr);
	LString type_name = type->GetName();
	PyTypeObject* typeobject = type->GetBindingType();
	type->mBindingModule = this;
	m_types[type_name] = typeobject;
	m_order_types.push_back(typeobject);
}

void LBindingModule::AddObject(const char* name, PyObject* obj)
{
	m_objects[name] = obj;
}

void LBindingModule::AddLObject(const char* name, LObject* val)
{
	PyObject* obj = to_binding(val);
	m_objects[name] = obj;
}

void LBindingModule::AddConstant(const char* name, long val)
{
	m_constants[name] = val;
}

bool LBindingModule::Init()
{
	if (m_inited)
		return true;
	m_module_def = {
	PyModuleDef_HEAD_INIT,
	m_module_name.c_str(),                       
	"luna core module", 
	0,
	NULL
	};
	GenerateDoc();
	m_binding_module = PyModule_Create(&m_module_def);
	auto dot = m_module_name.Find('.');
	auto pos = m_module_name.FindLast('.');
	if (pos != m_module_name.npos)
	{
		LString owner_module = m_module_name.Substr(0, pos);
		LString module_name = m_module_name.Substr(pos + 1, m_module_name.Length() - pos - 1);
		LBindingModule* lmodule = LBindingModule::Get(owner_module);
		if (!lmodule->m_inited)
			lmodule->Init();

		PyModule_AddObject(lmodule->m_binding_module, module_name.c_str(), (PyObject*)m_binding_module);
	}

	for (PyTypeObject* typeobject : m_order_types)
	{
		LType* type = LType::Get(typeobject);		
		assert(type != nullptr);
		type->GenerateBindingDoc();
		if(type->GetBindingMethods().size() > 1)
			typeobject->tp_methods = type->GetBindingMethods().data();
		if (type->GetBindingGetSet().size() > 1)
			typeobject->tp_getset = type->GetBindingGetSet().data();
		if (!sMetaType)
			InitMetaType();
		((PyObject*)typeobject)->ob_type = sMetaType;
		if (PyObject_HasAttrString(m_binding_module, typeobject->tp_name) == 1)
			continue;
		Py_INCREF(typeobject);
		if (PyType_Ready(typeobject) < 0)
		{
			PyErr_Print();
			assert(false);
		}
		assert(PyModule_AddObject(m_binding_module, typeobject->tp_name, (PyObject*)typeobject) == 0);
	}
	for (auto& it : m_objects)
	{
		const LString& type_name = it.first;
		PyObject* object = it.second;
		Py_INCREF(object);
		if (PyObject_HasAttrString(m_binding_module, type_name.c_str()) == 1)
			continue;
		assert(PyModule_AddObject(m_binding_module, type_name.c_str(), object) == 0);
	}
	for (auto& it : m_methods)
	{
		const LString& type_name = it.first;
		binding::Ref<PyObject, binding::KeepRef> method_name = PyUnicode_FromString(m_module_name.c_str());
		PyObject* func = PyCFunction_NewEx(&it.second, m_binding_module, method_name);
		PyModule_AddObject(m_binding_module, type_name.c_str(), func);
	}
	for (auto& it : m_constants)
	{
		const LString& type_name = it.first;
		long constant = it.second;
		assert(PyModule_AddIntConstant(m_binding_module, type_name.c_str(), constant) == 0);

	}

	m_inited = true;
	return true;
}

LBindingModule* LBindingModule::Get(const char* module_name)
{
	if (g_modules == nullptr)
		g_modules = new std::map<LString, LBindingModule*>();
	auto it = g_modules->find(module_name);
	LString module_name_str(module_name);
	if (it == g_modules->end())
	{
		LBindingModule* modu = new LBindingModule(module_name);
		return modu;
	}
	return it->second;
}

LBindingModule* LBindingModule::Luna()
{
	return LBindingModule::Get("luna");
}

}
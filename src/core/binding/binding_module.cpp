#include "core/binding/binding_module.h"
#include "core/math/math.h"
#include "core/binding/binding_traits.h"
#include "core/foundation/container.h"
#include "core/object/base_object.h"
#include "core/framework/module.h"

namespace luna
{

std::map<LString, BindingModule*>* sBindingModules;

void BindingModule::GenerateDoc()
{
	LString doc= "";
	for (auto method : mMethods)
	{
		if(method.second.ml_doc)
			doc = doc + method.second.ml_doc + "\n\n";
	}
	for (auto it : mConstants)
	{
		doc = doc + LString::Format("{} = {}\n\n", it.first, PyLong_AsSize_t(it.second));
	}

	for (auto it : mConstantStr)
	{
		Py_ssize_t size;
		doc = doc + LString::Format("{} = {}\n\n", it.first, PyUnicode_AsUTF8AndSize(it.second, &size));
	}
	mModuleDef.m_doc = LString::MakeStatic(doc);

}

PyObject* BindingModule::meta_type_new(PyTypeObject* metatype, PyObject* args, PyObject* kwds)
{
	PyTypeObject* pytype = (PyTypeObject*)PyType_Type.tp_new(metatype, args, kwds);
	LType* base = LType::Get(pytype->tp_base);
	LType* type = NewType(pytype->tp_name, pytype->tp_basicsize, LType::Get(pytype->tp_base));
	type->SetBindingType(pytype);
	type->mConstructable = base->mConstructable;
	type->mCtor = base->mCtor;
	type->mIsPureBindingType = true;
	return (PyObject*)pytype;
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
	ht->ht_name = PyUnicode_FromString("LMetaType");
	Py_ssize_t t;
	sMetaType->tp_name = PyUnicode_AsUTF8AndSize(ht->ht_name, &t);
	sMetaType->tp_as_number = &ht->as_number;
	sMetaType->tp_as_sequence = &ht->as_sequence;
	sMetaType->tp_as_mapping = &ht->as_mapping;
	sMetaType->tp_as_buffer = &ht->as_buffer;
	sMetaType->tp_new = (newfunc)&BindingModule::meta_type_new;	
	methods.emplace_back("get_properties",GetTypeProperties, METH_VARARGS, nullptr);
	methods.emplace_back(nullptr);
	sMetaType->tp_methods = methods.data();
	Py_INCREF(sMetaType);
	if (PyType_Ready(sMetaType) < 0)
	{
		assert(0);
	}
}


BindingModule::BindingModule(const LString& module_name)
{
	mModuleName = module_name;
	sBindingModules->operator [](module_name) = this;
}

void BindingModule::AddType(LType* type)
{
	assert(type->mBindingModule == nullptr);

	LString type_name = type->GetName();
	PyTypeObject* typeobject = type->GetBindingType();
	type->mBindingModule = this;
	mTypes[type_name] = typeobject;
	m_order_types.push_back(typeobject);

	if (mPythonModule)
	{
		_AddType(typeobject);
	}
}

void BindingModule::AddObject(const char* name, PyObject* object)
{	
	if (mPythonModule)
	{
		Py_INCREF(object);
		assert(PyModule_AddObject(mPythonModule, name, object) == 0);
	}else
	{
		mObjects[name] = object;
	}
}

void BindingModule::AddLObject(const char* name, LObject* val)
{
	PyObject* obj = to_binding(val);
	if (mPythonModule)
	{
		Py_INCREF(obj);
		assert(PyModule_AddObject(mPythonModule, name, obj) == 0);
	}
	else
	{
		mObjects[name] = obj;
	}	
}

void BindingModule::AddConstant(const char* name, const char* val)
{
	mConstantStr[name] = to_binding(val);
	if (mPythonModule)
	{
		assert(PyModule_AddObject(mPythonModule, name, mConstantStr[name]) == 0);
	}
}


void BindingModule::AddConstant(const char* name, long val)
{
	mConstants[name] = to_binding(val);
	if (mPythonModule)
	{
		assert(PyModule_AddIntConstant(mPythonModule, name, val) == 0);
	}
}

void BindingModule::AddSubModule(const char* name, BindingModule* bindingModule)
{
	assert(bindingModule->mParent == nullptr);
	bindingModule->mParent = this;
	mSubModules[name] = bindingModule;
}

void BindingModule::_AddType(PyTypeObject* typeobject)
{
	LType* type = LType::Get(typeobject);
	assert(type != nullptr);
	type->GenerateBindingDoc();
	if (type->GetBindingMethods().size() > 1)
		typeobject->tp_methods = type->GetBindingMethods().data();
	if (type->GetBindingGetSet().size() > 1)
		typeobject->tp_getset = type->GetBindingGetSet().data();
	if (!sMetaType)
		InitMetaType();
	((PyObject*)typeobject)->ob_type = sMetaType;
	if (PyObject_HasAttrString(mPythonModule, typeobject->tp_name) == 1)
		return;
	Py_INCREF(typeobject);
	if (PyType_Ready(typeobject) < 0)
	{
		PyErr_Print();
		assert(false);
	}
	assert(PyModule_AddObject(mPythonModule, typeobject->tp_name, (PyObject*)typeobject) == 0);
}

void BindingModule::_AddObject(const char* name, PyObject* object)
{
	if (PyObject_HasAttrString(mPythonModule, name) == 1)
		return;
}

bool BindingModule::Init()
{
	if (mPythonModule)
		return true;

	mModuleDef = {
		PyModuleDef_HEAD_INIT,
		mModuleName.c_str(),                       
		"luna module", 
		0,
		NULL
	};
	GenerateDoc();
	mPythonModule = PyModule_Create(&mModuleDef);
	auto dot = mModuleName.Find('.');
	auto pos = mModuleName.FindLast('.');
	if (pos != mModuleName.npos)
	{
		LString owner_module = mModuleName.Substr(0, pos);
		LString module_name = mModuleName.Substr(pos + 1, mModuleName.Length() - pos - 1);
		BindingModule* bindingModule = BindingModule::Get(owner_module);
		if (!bindingModule->mModuleInited)
			bindingModule->Init();
		PyModule_AddObject(bindingModule->mPythonModule, module_name.c_str(), (PyObject*)mPythonModule);
	}

	for (auto it : mSubModules)
	{
		it.second->Init();
	}

	for (PyTypeObject* typeobject : m_order_types)
	{
		_AddType(typeobject);
	}

	for (auto& it : mObjects)
	{
		AddObject(it.first.c_str(), it.second);
	}

	for (auto& it : mMethods)
	{
		const LString& type_name = it.first;
		binding::Ref<PyObject, binding::KeepRef> method_name = PyUnicode_FromString(mModuleName.c_str());
		PyObject* func = PyCFunction_NewEx(&it.second, mPythonModule, method_name);
		AddObject(type_name.c_str(), func);
	}

	for (auto& it : mConstants)
	{
		const LString& type_name = it.first;
		PyModule_AddObject(mPythonModule, type_name.c_str(), it.second);
	}

	for (auto& it : mConstantStr)
	{
		const LString& type_name = it.first;
		PyModule_AddObject(mPythonModule, type_name.c_str(), it.second);
	}
	return true;
}

BindingModule* BindingModule::Get(const char* name)
{
	LString moduleName = name;
	if (sBindingModules == nullptr)
		sBindingModules = new std::map<LString, BindingModule*>();
	LArray<LString> modulenames;
	BindingModule* parentModule = nullptr;
	moduleName.RSplitOnce(modulenames, ".");
	if (modulenames.size() > 0)
	{
		parentModule = BindingModule::Get(modulenames[0]);
	}
	auto it = sBindingModules->find(moduleName);
	
	if (it == sBindingModules->end())
	{
		auto m = new BindingModule(moduleName);
		if (parentModule)
			parentModule->AddSubModule(modulenames[1], m);
		return m;
	}
	return it->second;
}

BindingModule* BindingModule::Luna()
{
	return BindingModule::Get("luna");
}

}
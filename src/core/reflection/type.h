#pragma once

#include "core/reflection/method_traits.h"
#include "core/reflection/type_traits.h"
#include "core/reflection/method.h"
#include "core/reflection/property.h"


#include "core/foundation/string.h"
#include "core/foundation/container.h"

#include "core/binding/binding.h"
#include "core/binding/binding_traits.h"
#include "core/binding/binding_module.h"
#include "core/binding/binding_traits.h"

#include <stddef.h> 
#include <type_traits>
#include <functional>
#include <tuple>

namespace luna::binding
{

template<auto fn>
LString method_doc(const char* name);

}

namespace luna
{

template<typename T>
concept Reflecable = requires  { typename T::StaticClass; };

CORE_API extern PyTypeObject* sMetaType;
CORE_API extern std::map<LString, LType*>* sTypes;
CORE_API extern std::map<PyTypeObject*, LType*>* sBindingTypes;


class CORE_API LType
{
public:
	using PropertyMap = std::map<LString, LProperty>;
	using MethodMap = std::map<LString, LMethod>;

	LType(const LString& name, size_t size, LType* base = nullptr);
	LType(const LType&) = delete;
	LType(const LType&&) = delete;

	~LType() {};

public:
	bool IsSubPtrArray();
	bool IsAsset();
	bool IsObject();
	bool IsDerivedFrom(LType* base);

	inline size_t GetSize() { return mTypeSize; }
	inline LMethod& GetMethod(const LString& name);
	inline const LString& GetName() { return mTypeName; }
	const LString& GetBindingFullName();
	inline LType* GetTemplateArg() { return mTemplate; };
	inline bool Constructbale() const { return mConstructable; };

public:
	template<typename T>
	static void* DefaultCtor()
	{
		return new T();
	}	
	template<typename T>
	void Ctor()
	{
		mConstructable = true;
		mCtor = DefaultCtor<T>;
	}

	template<typename T>
	T* NewInstance()
	{
		//目前还不支持从C++侧构造Python的类实例
		LType* ret_type = LType::Get<T>();
		assert(IsDerivedFrom(ret_type));
		if (mCtor)
		{
			void* ptr = mCtor();
			T* obj = (T*)ptr;
			obj->SetType(this);
			return (T*)(ptr);
		}
		return nullptr;
	}

	template<auto fn>
	LMethod& Method(const char* name)
	{
		using FN = decltype(fn);
		using return_type = function_traits<FN>::return_type;
		using args_type = function_traits<FN>::args_type;
		//函数指针存储
		LMethod& func = mMethods[name];
		func.wrapper = new detail::method_wrapper<fn>();
		func.name = name;
		return mMethods[name];
	}
	template<auto fn>
	LMethod& BindingMethod(const char* name)
	{
		using FN = decltype(fn);
		auto it = mMethods.find(name);
		if (it == mMethods.end())
		{
			Method<fn>(name);
			it = mMethods.find(name);
		}
		it->second.Binding<fn>();
		return it->second;
	}

	//无实际存储内存的一个属性
	LProperty& VirtualProperty(const char* name)
	{
		LProperty& proerpty = mProperties[name];
		proerpty.SetPropertyName(name);
		proerpty.mOwner = this;
		 
		return mProperties[name];
	}

	template<auto mem>
	LProperty& Property(const char* name)
	{
		using M = decltype(mem);
		using class_type = member_pointer_traits<M>::class_type;
		using member_type = member_pointer_traits<M>::member_type;

		using member_raw_type = std::remove_pointer_t<member_type>;

		LProperty& proerpty = mProperties[name];

		proerpty.mIsSubPtr = std::is_base_of<luna::LSubPtr, member_raw_type>::value;
		proerpty.mType = LType::Get<member_raw_type>();
		proerpty.SetPropertyName(name);

		proerpty.mOffset = reinterpret_cast<size_t>(&((class_type*)(0)->*mem));
		proerpty.mSize = sizeof(member_type);
		proerpty.mOwner = this;
		return mProperties[name];
	}

	template<auto mem>
	LProperty& BindingProperty(const char* name)
	{
		using M = decltype(mem);
		using class_type = member_pointer_traits<M>::class_type;
		using member_type = member_pointer_traits<M>::member_type;
		LProperty& prop = Property<mem>(name);
		prop.Binding<class_type, member_type>();
		return prop;
	}

	template<typename T>
	void Binding()
	{
		mPyType = new PyTypeObject();

		if (sBindingTypes == nullptr)
			sBindingTypes = new std::map<PyTypeObject*, LType*>();

		(*sBindingTypes)[mPyType] = this;

		PyTypeObject& binding_type = *mPyType;

		binding_type = {
			PyVarObject_HEAD_INIT(&PyType_Type, 0)
		};

		binding_type.tp_name = GetName().c_str();
		binding_type.tp_base = mBase ? mBase->mPyType : nullptr;		
		binding_type.tp_basicsize = sizeof(binding::binding_proxy<T>::binding_object_t);
		binding_type.tp_itemsize = 0;


		binding_type.tp_getattro = binding::binding_proxy<T>::get_getattrofunc();
		binding_type.tp_setattro = binding::binding_proxy<T>::get_setattrofunc();
		binding_type.tp_flags = binding::binding_proxy<T>::get_type_flags();
		binding_type.tp_init = binding::binding_proxy<T>::get_initproc();
		binding_type.tp_dealloc = binding::binding_proxy<T>::get_destructor();
		binding_type.tp_new = binding::binding_proxy<T>::get_newfunc();

	}

	MethodMap& GetMethods() { return mMethods; }
	PropertyMap& GetProperties() { return mProperties; }
	LProperty* GetProperty(const char* value);
	void GetAllProperties(std::vector<LProperty*>& result);

	template<typename T>
	static LType* Get()
	{
		return static_type<std::decay_t<T>>::StaticType();
	}

	static LType* Get(LString& name)
	{
		return sTypes->operator[](name);
	}

	static LType* Get(PyTypeObject* binding_type)
	{
		if (sBindingTypes == nullptr)
			sBindingTypes = new std::map<PyTypeObject*, LType*>();
		return sBindingTypes->operator [](binding_type);
	}

	static LType* StaticType();
	// Binding
public:
	PyObject* GetBindingAttr(const char* name) const
	{
		return nullptr;
	}

	PyTypeObject* GetBindingType() const
	{
		if (mPyType)
			return mPyType;
		if (mBase)
			return mBase->GetBindingType();
		// No Binding??
		assert(false);
		return nullptr;
	};

	PyTypeObject* mPyType = nullptr;

	LType* GetBase() const noexcept { return mBase; }

	//生成Binding的文档
	void GenerateBindingDoc();
	//是否是Python binding
	bool IsPureBindingType() const noexcept { return mIsPureBindingType; }
	//是否 C++ native
	bool IsNativeType() const noexcept { return mIsPureBindingType; }

	std::vector<LString>& GetExtraDocs() { return mExtraDocs; }


	void SetBindingType(PyTypeObject* binding_type)
	{
		mPyType = binding_type;
		sBindingTypes->operator [](binding_type) = this;
	}
protected:

	std::vector<PyMethodDef>& GetBindingMethods()
	{
		mMethodDefCache.clear();
		for (auto it : mMethods)
		{
			LMethod& method = it.second;
			mMethodDefCache.push_back(method.GetBindingMethodDef());
		}
		PyMethodDef def;
		def.ml_name = nullptr;
		mMethodDefCache.push_back(def);
		return mMethodDefCache;
	};

	std::vector<PyGetSetDef>& GetBindingGetSet()
	{
		mPropDefCache.clear();
		for (auto it : mProperties)
		{
			LProperty& prop = it.second;
			if(prop.HasBinding())
				mPropDefCache.push_back(prop.GetBindingDef());
		}
		PyGetSetDef def;
		def.name = nullptr;
		mPropDefCache.push_back(def);
		return mPropDefCache;
	}

	bool                         mIsPureBindingType = false;


	std::map<LString, PyObject*> mCallableCache;
	std::vector<PyMethodDef>     mMethodDefCache;
	std::vector<PyGetSetDef>     mPropDefCache;

	std::function<void* ()>      mCtor;
	bool                         mConstructable         = false;

private:
	LType*               mBase          = nullptr;
	LType*               mTemplate      = nullptr;

	PropertyMap          mProperties;
	MethodMap            mMethods;

	//额外的binding属性和binding method，以及binding method doc
	std::vector<LString> mExtraDocs;

	size_t               mTypeSize      = 0;
	LString              mTypeName;
	LString              mFullName;
	BindingModule*       mBindingModule = nullptr;


	template<typename first>
	friend LType* NewTemplateType(const LString& name, size_t size, LType* base);

	friend class BindingModule;
	friend class LObject;

};

template<typename first>
LType* NewTemplateType(const LString& name, size_t size, LType* base)
{
	LString temp_name = LString::Format("{0}<{1}>", name, LType::Get<first>()->GetName());
	LType* type = NewType(temp_name, size, base);
	type->mTemplate = LType::Get<first>();
	return type;
}

}

namespace luna::binding
{


template<>
struct binding_converter<LType*>
{
	inline static PyObject* to_binding(LType* val)
	{
		PyTypeObject* obj = val->GetBindingType();
		Py_XINCREF(obj);
		return (PyObject*)obj;
	}

	inline static LType* from_binding(PyObject* obj)
	{
		return LType::Get((PyTypeObject*)obj);
	}

	static const char* binding_fullname()
	{
		return "type";
	}
};

}
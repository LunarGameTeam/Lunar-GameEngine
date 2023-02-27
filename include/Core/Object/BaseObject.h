#pragma once


#include "Core/CoreTypes.h"

#include "Core/Foundation/Container.h"
#include "Core/Memory/Ptr.h"
#include "Core/Reflection/Type.h"
#include "Core/Scripting/BindingTraits.h"
#include "Core/Reflection/Reflection.h"

#include <memory>


namespace luna::binding
{
struct BindingLObject;
}

namespace luna
{

class CORE_API LObject
{
	RegisterTypeEmbedd(LObject, InvalidType)

protected:
	LObject();
	virtual ~LObject();

public:
	using binding_t = binding::BindingLObject;
	/// 
	/// <summary>
	/// 序列化反序列化
	/// </summary>
	/// 
	virtual void Serialize(Serializer& serializer);
	virtual void DeSerialize(Serializer& serializer);

public:
	/// 
	/// <summary>
	/// 基本接口
	/// </summary>
	/// 
	inline const LString& GetObjectName()
	{
		return mName;
	}
	inline void SetObjectName(LString& val)
	{
		mName = val;
	}

	virtual LType* GetClass() const
	{		
		return mObjectType;
	}

	LObject* GetParent();		
	
public:
	/// 
	/// <summary>
	/// binding
	/// </summary>
	/// 
	template<typename... Args>
	void InvokeBinding(const char* N, Args&& ...args)
	{
		LString name = N;
		auto cls = GetClass();

		auto it = cls->mCallableCache.find(name);
		binding::Ref<PyObject, binding::KeepRef> self = to_binding(this);

		binding::Ref<PyObject, binding::KeepRef> method_name = PyUnicode_FromString(name);
		PyObject* obj = method_name;
		binding::Ref<PyObject, binding::KeepRef> attr = PyObject_GetAttr(self, method_name);
		if (PyCallable_Check(attr))
		{
			Py_XINCREF(attr);
			cls->mCallableCache[name] = attr;
			PyObject_CallFunctionObjArgs(attr, to_binding(args)..., NULL);
			PyErr_Print();
		}
		return;
	}

	void SetType(LType* type) 
	{
		mObjectType = type;
	}

	inline size_t GetInstanceID() const { return mInstanceID; }
	static LObject* InstanceIDToObject(size_t id);

	binding::BindingLObject* GetBindingObject() { return (binding::BindingLObject*)(mBindingObject); }
	virtual void SetBindingObject(PyObject* val);

	void SetParent(LObject* obj);

	void ForEachSubObject(std::function<void(size_t, LObject*)> func);

	size_t Index() const;

	const LList<LObject*>& GetSubObejcts() { return mSubObjects; }

	bool IsSerializable() const { return mSerializable; }
	void SetSerializable(bool val) { mSerializable = val; }

protected:
	void* operator new(size_t size)
	{
		void* p = ::operator new(size);
		return p;
	}

protected:
	bool            mSerializable = true;
	LString         mName         = "";
	LType*          mObjectType   = nullptr;
	LList<LObject*> mSubObjects;
	LObject*        mParent       = nullptr;
	uint64_t        mInstanceID   = 0;
protected:
	binding::BindingObject* mBindingObject = nullptr;

private:
	template<typename RET>
	friend RET* NewObject(LType* obj_type);

	template<typename ObjectType>
	friend ObjectType* TCreateObject();

	friend class LType;
};

template<typename RET>
RET* NewObject(LType* type = LType::Get<RET>())
{
	assert(type->Constructbale());
	LObject* obj = type->NewInstance<LObject>();	
	return (RET*)obj;
}

template<typename ObjectType>
ObjectType* TCreateObject()
{	
	ObjectType* obj = new ObjectType();
	return obj;
}

}

//Binding相关

namespace luna::binding
{

struct CORE_API BindingLObject  : public BindingObject
{
	TPPtr<LObject> ptr;

	LObject* GetPtr() { return ptr.Get(); }

	BindingLObject()
	{
		sBindingObjectNum++;
	}

	~BindingLObject()
	{
		sBindingObjectNum--;
	}

	static size_t sBindingObjectNum;

	static PyObject* __alloc__(PyTypeObject* type, Py_ssize_t size);
	static void __destrctor__(PyObject* self);
	static int __bool__(PyObject* self);
	static PyObject* __new__(PyTypeObject* type, PyObject* args, PyObject* kwrds);
	
	static int __init__(PyObject* self, PyObject* args, PyObject* kwrds)
	{	
		return 0;
	}
};


template<typename T>
struct binding_proxy<T, typename std::enable_if_t<std::is_base_of_v<LObject, T>>> : binding_proxy_base
{
	using binding_object_t = typename T::binding_t;

	static int get_type_flags() { return Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE; };
		
	constexpr static destructor get_destructor = binding_object_t::__destrctor__;
	constexpr static allocfunc get_allocfunc = binding_object_t::__alloc__;

	static constexpr newfunc get_newfunc = binding_object_t::__new__;
	constexpr static initproc get_initproc = binding_object_t::__init__;
	constexpr static inquiry get_bool = binding_object_t::__bool__;

	template<typename M>
	static PyObject* raw_getter(PyObject* s, void* closure)
	{		
		typename T::binding_t* o = static_cast<binding_object_t*>(s);
		LObject* obj = o->GetPtr();
		if (!obj)
			Py_RETURN_NONE;
		LProperty& prop = *(LProperty*)(closure);
		size_t offset = prop.GetOffset();
		M* mem_ptr = (M*)((char*)obj + offset);
		M& mem = *mem_ptr;
		return binding_converter<M>::to_binding(mem);
	}

	template<typename M>
	static int raw_setter(PyObject* s, PyObject* val, void* closure)
	{
		typename T::binding_t* o = static_cast<binding_object_t*>(s);
		LObject* obj = o->GetPtr();
		if (!obj)
			return -1;
		LProperty& prop = *static_cast<LProperty*>(closure);
		const size_t offset = prop.GetOffset();
		M* mem_ptr = (M*)((char*)obj + offset);
		*mem_ptr = binding_converter<M>::from_binding(val);
		return 0;
	}
};

template<typename U>
struct binding_converter<U*, typename  std::enable_if_t<std::is_base_of_v<LObject, U>>>
{
	using binding_object_t = typename U::binding_t;

	static PyObject* to_binding(LObject* obj)
	{
		PyTypeObject* typeobject = LType::Get<U>()->GetBindingType();
		if (obj)
		{
			PyObject* bindingObject = nullptr;
			typeobject = obj->GetClass()->GetBindingType();
			bindingObject = obj->GetBindingObject();
			if (bindingObject)
			{
				Py_XINCREF(bindingObject);
				return bindingObject;
			}
			// first time to binding...
			bindingObject = typeobject->tp_alloc(typeobject, 0);
			obj->SetBindingObject(bindingObject);
			return  bindingObject;
		}
		Py_RETURN_NONE;		
	}

	static U* from_binding(PyObject* obj)
	{
		binding_object_t* res = static_cast<binding_object_t*>(obj);
		return static_cast<U*>(res->GetPtr());
	}

	static const char* binding_fullname()
	{
		return LType::Get<U>()->GetBindingFullName();
	}

};


template<typename U>
struct binding_converter<TPPtr<U>>
{
	using T = U;

	inline static PyObject* to_binding(TPPtr<U>& pptr)
	{
		LObject* obj = pptr.Get();
		BindingLObject* bindingObject = static_cast<BindingLObject*>(binding_converter<U*>::to_binding(obj));
		return (PyObject*)bindingObject;
	}

	inline static T* from_binding(PyObject* obj)
	{
		BindingLObject* res = ( BindingLObject*)(obj);
		return (T*)res->ptr.Get();
	}

	static const char* binding_fullname()
	{
		return LType::Get<T>()->GetBindingFullName();
	}
};

template<typename U>
struct binding_converter<TPPtrArray<U>>
{

	static PyObject* to_binding(const TPPtrArray<U>& array)
	{
		PyObject* res = PyList_New(array.Size());
		for (auto& it: array)
		{
			PyList_Append(res, binding_converter<U*>::to_binding(it.Get()));
		}
		return (PyObject*)res;
	}

	static TPPtrArray<U> from_binding(PyObject* obj)
	{
		assert(false);
		TPPtrArray<U> u;
		return u;
	}

	static const char* binding_fullname()
	{
		return LType::Get<U>()->GetBindingFullName();
	}
};
}
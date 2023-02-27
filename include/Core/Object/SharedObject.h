#pragma once


#include "Core/Foundation/Container.h"
#include "Core/Object/BaseObject.h"
#include "Core/Memory/Ptr.h"
#include "Core/Reflection/Type.h"

namespace luna
{
namespace  binding
{
struct BindingSharedObject;
}

template<typename T>
using SharedPtr = std::shared_ptr<T>;

template<typename T>
SharedPtr<T> ToSharedPtr(T* val)
{
	return std::dynamic_pointer_cast<T, SharedObject>(val->shared_from_this());
}

class CORE_API SharedObject : public LObject, public std::enable_shared_from_this<SharedObject>
{	
	RegisterTypeEmbedd(SharedObject, LObject)
public:
	using binding_t = binding::BindingSharedObject;
	void SetBindingObject(PyObject* val) override;
};


namespace  binding
{

struct CORE_API BindingSharedObject : public BindingObject
{
	std::shared_ptr<SharedObject> ptr;
	SharedObject* GetPtr() { return ptr.get(); }

	BindingSharedObject() = default;	
	~BindingSharedObject() = default;
	

	static PyObject* __alloc__(PyTypeObject* type, Py_ssize_t size);
	static void __destrctor__(PyObject* self);
	static int __bool__(PyObject* self);
	static PyObject* __new__(PyTypeObject* type, PyObject* args, PyObject* kwrds);
	
	static int __init__(PyObject* self, PyObject* args, PyObject* kwrds)
	{
		return 0;
	}
};


template<typename U>
struct binding_converter<SharedPtr<U>>
{
	inline static PyObject* to_binding(SharedPtr<U> pptr)
	{
		LObject* obj = pptr.get();
		BindingSharedObject* bindingObject = static_cast<BindingSharedObject*>(binding_converter<U*>::to_binding(obj));
		return (PyObject*)bindingObject;
	}

	inline static SharedPtr<U> from_binding(PyObject* obj)
	{
		BindingSharedObject* res = (BindingSharedObject*)(obj);
		return std::dynamic_pointer_cast<U, SharedObject>(res->ptr);
	}

	static const char* binding_fullname()
	{
		return LType::Get<U>()->GetBindingFullName();
	}
};




}
}
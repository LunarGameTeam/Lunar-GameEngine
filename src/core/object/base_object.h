#pragma once



#include "core/memory/garbage_collector.h"
#include "core/foundation/container.h"
#include "core/foundation/uuid.h"
#include "core/reflection/reflection.h"

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>

#include <memory>

#include "core/binding/binding_traits.h"
#include "core/reflection/reflection.h"

namespace luna::binding
{
struct  BindingLObject;
}

namespace luna
{
using LUuid = boost::uuids::uuid;

class LObject;
class LStruct;
class AssetModule;
struct WeakPtrHandle;


//默认Json
class CORE_API ISerializer
{
public:
	ISerializer() = default;
	virtual bool DeSerialize(LObject* obj) = 0;
	virtual bool Serialize(LObject* obj) = 0;
	virtual bool DeSerialize(LStruct* obj) { return true; };

};

static LObject* lroot_object;

/*!
 * \class Object
 *
 * \brief
 *
 * \author
 *
 */


class CORE_API LStruct
{
public:
	void Serialize(ISerializer& serializer)
	{
		serializer.DeSerialize(this);
	};
};

class CORE_API LObject
{
	RegisterTypeEmbedd(LObject, InvalidType)
public:
	virtual ~LObject() noexcept;
	/// 
	/// <summary>
	/// 内存
	/// </summary>
	/// 
	virtual LObject*& AllocateSubSlot();
	virtual void DeAllocateSubSlot(LObject** obj);
	inline WeakPtrHandle* GetHandle() const noexcept { return mHandle; }

public:
	/// 
	/// <summary>
	/// 序列化反序列化
	/// </summary>
	/// 
	virtual void Serialize(ISerializer& serializer);
	virtual void DeSerialize(ISerializer& serializer);

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

	binding::BindingLObject* GetBindingObject() { return mBindingObject; }
	void SetBindingObject(PyObject* val);

protected:
	void* operator new(size_t size)
	{
		void* p = ::operator new(size);
		LGcCore::Instance()->HostObject(static_cast<LObject*>(p));
		return p;
	}
	LObject();

protected:
	LString mName;
	LType*  mObjectType = nullptr;
protected:
	LList<LObject*> mSubObjects;
	LObject*        mParent = nullptr;

private:	
	uint64_t                 mInstanceID;
	WeakPtrHandle*           mHandle;
	TWeakPtr<LObject>        mSelfHandle;
	binding::BindingLObject* mBindingObject = nullptr;

private:
	template<typename RET>
	friend RET* NewObject(LType* obj_type);

	template<typename ObjectType>
	friend ObjectType* TCreateObject();

	friend class LType;
	friend class SceneSerializer;
	friend class JsonSerializer;
	friend class AssetModule;
	
	template<typename T>
	friend struct binding_proxy;
};

template<typename RET>
RET* NewObject(LType* type = LType::Get<RET>())
{
	assert(type->Constructbale());
	LObject* obj = type->NewInstance<LObject>();	
	obj->SetType(type);
	return (RET*)obj;
}

template<typename ObjectType>
ObjectType* TCreateObject()
{	
	ObjectType* obj = new ObjectType();
	return obj;
}
}


namespace luna::binding
{

struct CORE_API BindingLObject  : public BindingObject
{
	TWeakPtr<LObject> ptr;

	LObject* GetPtr() { return ptr.get(); }

	BindingLObject()
	{
		sBindingObjectNum++;
	}

	~BindingLObject()
	{
		sBindingObjectNum--;
	}

	static size_t sBindingObjectNum;
};


template<typename T>
struct binding_proxy<T, typename std::enable_if_t<std::is_base_of_v<LObject, T>>> : binding_proxy_base
{
	using binding_object_t = BindingLObject;

	static int get_type_flags() { return Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE; };

	static void LObject_destructor(PyObject* self)
	{
		PyTypeObject* tp = Py_TYPE(self);
		BindingLObject* obj = (BindingLObject*)(self);
		obj->~BindingLObject();
		tp->tp_free(self);
	}
	static destructor get_destructor() { return LObject_destructor; }

	static PyObject* LObject_allocfunc(PyTypeObject* type, Py_ssize_t size)
	{
		BindingLObject* bindingObject = PyType_GenericAlloc(type, size);
		std::allocator<TWeakPtr<LObject>>::construct(&(bindingObject->ptr));
		return bindingObject;
	}

	static allocfunc get_allocfunc() { return LObject_allocfunc; }

	static PyObject* LObject_newfunc(PyTypeObject* type, PyObject* args, PyObject* kwrds)
	{
		Py_XINCREF(type);
		LType* object_type = LType::Get(type);
		BindingLObject* obj = (BindingLObject*)type->tp_alloc(type, 0);
		T* t = object_type->NewInstance<T>();		

#ifdef _DEBUG
		PyObject* dict = PyObject_GenericGetDict(obj, nullptr);
		//Check一下Dict
		assert(PyDict_Check(dict));
		Py_XDECREF(dict);
#endif

		t->SetType(object_type);
		t->SetBindingObject(obj);
		return (PyObject*)obj;
	}
	static newfunc get_newfunc() { return LObject_newfunc; }

	template<typename M>
	static PyObject* raw_getter(PyObject* s, void* closure)
	{
		BindingLObject* o = ( BindingLObject*)(s);
		LObject* obj = o->ptr.get();
		LProperty& prop = *(LProperty*)(closure);
		size_t offset = prop.GetOffset();
		M* mem_ptr = (M*)((char*)obj + offset);
		M& mem = *mem_ptr;
		return binding_converter<M>::to_binding(mem);
	}

	template<typename M>
	static int raw_setter(PyObject* s, PyObject* val, void* closure)
	{
		BindingLObject* o = ( BindingLObject*)(s);
		LObject* obj = o->ptr.get();
		LProperty& prop = *(LProperty*)(closure);
		size_t offset = prop.GetOffset();
		M* mem_ptr = (M*)((char*)obj + offset);
		*mem_ptr = binding_converter<M>::from_binding(val);
		return 0;
	}
};

template<typename U>
struct binding_converter<U*> : std::enable_if_t<std::is_base_of_v<LObject, U>>
{
	using T = U;

	inline static PyObject* to_binding(LObject* obj)
	{
		BindingLObject* bindingObject;
		PyTypeObject* typeobject = LType::Get<T>()->GetBindingType();
		if (obj)
		{
			typeobject = obj->GetClass()->GetBindingType();
			if (obj->GetBindingObject())
			{
				Py_XINCREF(obj->GetBindingObject());
				return obj->GetBindingObject();
			}
			// first time to binding...
			bindingObject = (BindingLObject*) typeobject->tp_alloc(typeobject, 0);
			obj->SetBindingObject((PyObject*)bindingObject);
		}
		else
		{
			bindingObject = (BindingLObject*) typeobject->tp_alloc(typeobject, 0);
			bindingObject->ptr = obj;
		}

		return (PyObject*)bindingObject;
	}
	inline static T* from_binding(PyObject* obj)
	{
		 BindingLObject* res = ( BindingLObject*)(obj);		
		return (T*) res->ptr.get();
	}

	static const char* binding_fullname()
	{
		return LType::Get<T>()->GetBindingFullName();
	}

};

template<typename U>
struct binding_converter<TSubPtr<U>>
{
	using T = U;

	inline static PyObject* to_binding(TSubPtr<U>& sub_ptr)
	{
		LObject* obj = sub_ptr.Get();
		 BindingLObject* bindingObject;
		if (obj)
		{
			if (obj->GetBindingObject())
			{
				Py_XINCREF(obj->GetBindingObject());
				return (PyObject*)obj->GetBindingObject();
			}
			// first time to binding...
			bindingObject = PyObject_New( BindingLObject, obj->GetClass()->GetBindingType());
			obj->SetBindingObject((PyObject*)bindingObject);
		}
		else
		{
			bindingObject = PyObject_New( BindingLObject, LType::Get<T>()->m_binding_type);
			memset(&bindingObject->ptr, 0, sizeof(TWeakPtr<LObject>));
			bindingObject->ptr = obj;
		}
		return (PyObject*)bindingObject;
	}

	inline static T* from_binding(PyObject* obj)
	{
		 BindingLObject* res = ( BindingLObject*)(obj);
		return (T*)res->ptr.get();
	}

	static const char* binding_fullname()
	{
		return LType::Get<T>()->GetBindingFullName();
	}
};

}
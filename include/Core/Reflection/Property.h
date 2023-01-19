#pragma once


#include "Core/Foundation/String.h"
#include "Core/Reflection/Method.h"

#include "Core/Reflection/MethodTraits.h"
#include "Core/Scripting/BindingTraits.h"
#include "Core/Reflection/TypeTraits.h"

#include <tuple>
#include <type_traits>
#include <utility>

namespace luna
{

class LType;
class LProperty;


enum class PropertyFlag : uint32_t
{
	None = 0,
	Serializable = 1 << 0,
	HasBinding = 1 << 1,
};

ENABLE_BITMASK_OPERATORS(PropertyFlag);

class CORE_API LProperty  : public binding::BindingProperty
{
	RegisterTypeEmbedd(LProperty, InvalidType);
public:
	LProperty();
	inline const char *GetNameStr() const { return mName.c_str(); }
	inline const LString &GetName() const { return mName; }
	inline uint32_t GetSize() const { return mSize; }
	inline size_t GetOffset() const { return mOffset; }
	inline bool IsSubPointer() const { return mIsSubPtr; }
	inline LType* GetType() const { return mType; }
	
	inline bool HasBinding() const { return mHasBinding; }
	inline LType* GetOwnerType() { return mOwner; }
	inline LMethod& GetSetter() { return mSetter; }
	inline LMethod& GetGetter() { return mGetter; }


	inline bool CheckFlag(PropertyFlag info) const { return int(mFlags) & (int)(info); }

	inline LProperty& Serialize()
	{
		return SetFlag(PropertyFlag::Serializable);		 
	}

	inline LProperty& SetFlag(PropertyFlag val)
	{
		mFlags = (PropertyFlag)((int)mFlags | (int)val);
		return *this;
	}

	template<typename T>
	void Type()
	{
		mType = static_type<T>::StaticType();
	}

	template<auto fn>
	LProperty& Getter()
	{
		using FN = decltype(fn);
		//函数指针存储	
		function_pointer_container<FN> p;
		p.fn = fn;
		mGetter.ptr = p.ptr;
		mGetter.name = mName;
		//使用成员函数作为Getter
		if constexpr (std::is_member_function_pointer<FN>::value)
		{
			using class_type = function_traits<FN>::class_type;
			using return_type = function_traits<FN>::return_type;
			mBindingDef.get = (getter)BindingGetter<return_type, class_type>;
		}
		//使用普通函数作为Getter
		else
		{						
			using args_type = function_traits<FN>::args_type;
			static_assert(std::is_pointer_v<std::tuple_element_t< 0, args_type>>);
			using class_type = std::remove_pointer_t< std::tuple_element_t< 0, args_type> >;
			//假定第一个参数是class type
			using return_type = function_traits<FN>::return_type;
			mBindingDef.get = (getter)BindingGetter<return_type, class_type>;
		}		
		return *this;
	}

	template<auto fn>
	LProperty &Setter()
	{
		using FN = decltype(fn);
		if constexpr (std::is_member_function_pointer<FN>::value)
		{
			using class_type = function_traits<FN>::class_type;
			using args_type = function_traits<FN>::args_type;
			mBindingDef.set = (setter)BindingSetter<class_type, std::tuple_element_t<0, args_type>>;
		}
		else
		{
			using args_type = function_traits<FN>::args_type;
			static_assert(std::is_pointer_v<std::tuple_element_t< 0, args_type>>);
			using class_type = std::remove_pointer_t< std::tuple_element_t< 0, args_type> >;
			mBindingDef.set = (setter)BindingSetter<class_type, std::tuple_element_t<1, args_type>>;
		}

		
		//函数指针存储	
		function_pointer_container<FN> p;
		p.fn = fn;
		mSetter.ptr = p.ptr;
		mSetter.name = mName;

		return *this;
	}

	template<typename class_type, typename M>
	LProperty& Binding()
	{
		mType = static_type< std::remove_pointer_t<std::decay_t<M>>>::StaticType();
		if (mHasBinding)
			return *this;

		mHasBinding = true;
		const char* type_name = binding::binding_converter<M>::binding_fullname();
		
		mBindingDef.doc = LString::MakeStatic(LString::Format("{0}: {1}", mName, type_name));


		if (!mBindingDef.get)
			mBindingDef.get = (getter)binding::binding_proxy<class_type>::template raw_getter<M>;

		if (!mBindingDef.set)
			mBindingDef.set = (setter)binding::binding_proxy<class_type>::template raw_setter<M>;

		mFlags = mFlags | PropertyFlag::HasBinding;

		return *this;

	}

	template<typename T, typename Cls>
	T& GetValue(Cls* instance) const
	{
		byte* p = (byte*)instance + mOffset;
		T* result = (T*)p;
		return *result;
	}

	template<typename Cls, typename T>
	void SetValue(Cls* instance, const T& value)
	{
		if (mSetter)
		{
			mSetter.InvokeMember<void, Cls>(instance, value);
		}
		else
		{
			byte* p = (byte*)instance + mOffset;
			T* result = (T*)p;
			*result = value;
		}
	}

	PyGetSetDef& GetBindingDef() { return mBindingDef; }

protected:
	void SetPropertyName(const char* name);

	template<typename Ret, typename Cls>
	static PyObject* BindingGetter(PyObject* obj, void* closure)
	{
		using binding_object_t =	binding::binding_proxy<Cls>::binding_object_t;
		binding_object_t* binding_wrap = (binding_object_t*)(obj);
		Cls* self = (Cls*)(binding_wrap->GetPtr());
		LProperty* mType = (LProperty*)(closure);
		assert(mType->GetGetter());
		if (!self)
			Py_RETURN_NONE;
		Ret ret = mType->GetGetter().InvokeMember<Ret, Cls>(self);
		return to_binding<Ret>(ret);
	}

	template<typename Cls, typename Arg>
	static int BindingSetter(PyObject* obj, PyObject* arg, void* closure)
	{
		using binding_object_t = binding::binding_proxy<Cls>::binding_object_t;
		binding_object_t* binding_wrap = (binding_object_t*)(obj);
		Cls* self =(Cls*)(binding_wrap->GetPtr());
		LProperty* mType = (LProperty*)(closure);
		assert(mType->GetSetter());
		auto value = from_binding<Arg>(arg);
		mType->GetSetter().InvokeMember<void, Cls, Arg>(self, value);
		return 0;
	}
private:
	bool         mHasBinding = false;
	bool         mDefing     = true;
	LType*       mType       = nullptr;
	LType*       mOwner      = nullptr;

	LString      mName;
	bool         mIsSubPtr   = false;
	PropertyFlag mFlags      = PropertyFlag::None;

	LMethod      mSetter;
	LMethod      mGetter;
	friend class LType;
};

namespace binding
{
template<> struct binding_converter<LProperty*> : native_converter<LProperty> { };
template<> struct binding_proxy<LProperty> : native_binding_proxy<LProperty> { };

}


}

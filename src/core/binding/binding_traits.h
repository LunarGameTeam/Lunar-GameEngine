#pragma once

#include "core/binding/binding_python.h"

#include "core/core_types.h"
#include "core/reflection/method_traits.h"
#include "core/reflection/type_traits.h"
#include "core/math/math.h"
#include "core/foundation/string.h"

#include <utility>
#include <type_traits>
#include <concepts>
#include <functional>
#include <tuple>


namespace luna::binding
{

template<typename T>
struct binding_type
{
	static PyTypeObject* StaticType()
	{
		static PyTypeObject* type = static_type<T>::StaticType()->GetBindingType();
		return type;
	}
};

enum class IncRef
{ };
enum class KeepRef
{ };

template<typename T, typename P>
class Ref;

template<typename T>
class Ref<T, IncRef>
{
public:
	Ref(PyObject* _obj)
		:obj(_obj)
	{
		Py_XINCREF(obj);
	}

	Ref(Ref& val)
	{
		obj = val.obj;
	}

	~Ref()
	{
		Py_XDECREF(obj);
	}

	operator PyObject* () const
	{
		return obj;
	}

	void operator=(PyObject* _obj)
	{
		obj = _obj;
		Py_XINCREF(obj);
	}

	T* operator->()
	{
		return obj;
	}

private:
	T* obj;


};

template<typename T>
class Ref<T, KeepRef>
{
public:
	Ref(PyObject* _obj)
		:obj(_obj)
	{

	}

	Ref(Ref& val)
	{
		obj = val.obj;
	}

	~Ref()
	{
		Py_XDECREF(obj);
	}

	operator PyObject* () const
	{
		return obj;
	}

	void operator=(PyObject* _obj)
	{
		obj = _obj;
	}

	T* operator->()
	{
		return obj;
	}

private:
	T* obj;
};

#define STATIC_BINDING_STR(var, str) static PyObject* var = PyUnicode_FromString(#str);

template<typename> struct pymethod_wrap;

template<typename> struct binding_converter
{
	static PyObject* binding_type() { return nullptr; }
};

template<typename T>
struct struct_converter
{
	inline static PyObject* to_binding(const T& obj)
	{
		BindingStruct<T>* wrap = PyObject_New(BindingStruct<T>, binding_type<T>::StaticType());
		wrap->val = obj;
		return (PyObject*)wrap;
	}
	inline static T from_binding(PyObject* obj)
	{
		BindingStruct<T>* res = (BindingStruct<T>*)(obj);
		return res->val;
	}
	static const char* binding_fullname()
	{
		return static_type<T>::StaticType()->GetBindingFullName();
	}
	
};


template<typename T>
struct native_converter
{	
	inline static PyObject* to_binding(T* obj)
	{
		BindingNative<T>* wrap = PyObject_New(BindingNative<T>, binding_type<T>::StaticType());
		wrap->val = obj;
		return (PyObject*)wrap;
	}

	inline static T* from_binding(PyObject* obj)
	{
		BindingNative<T>* res = (BindingNative<T>*)(obj);
		return res->val;
	}

	static const char* binding_fullname()
	{
		return static_type<T>::StaticType()->GetBindingFullName();
	}
};

class BindingProperty
{

public:
	uint32_t mSize = 0;
	size_t mOffset = 0;
	PyGetSetDef mBindingDef;
};
template<>
struct binding_converter<int>
{
	inline static PyObject* to_binding(int val) 
	{		
		return PyLong_FromLong(val);
	}

	inline static int from_binding(PyObject* obj)
	{
		assert(PyLong_Check(obj));
		long res = PyLong_AsLong(obj);
		return res;			
	}

	static const char* binding_fullname()
	{
		return "int";
	}
};

template<>
struct binding_converter<void>
{
	inline static PyObject* to_binding(void)
	{
		Py_RETURN_NONE;
	}

	inline static void from_binding(PyObject* obj)
	{		
	}

	static const char* binding_fullname()
	{
		return "None";
	}
};

template<>
struct binding_converter<bool>
{
	inline static PyObject* to_binding(bool val)
	{		
		return PyBool_FromLong(val);
	}

	inline static bool from_binding(PyObject* obj)
	{
		return Py_IsTrue(obj) ? true : false;
	}

	static const char* binding_fullname()
	{
		return "bool";
	}
};

template<>
struct binding_converter<void*>
{
	inline static PyObject* to_binding(const void* val)
	{
		size_t repr = reinterpret_cast<size_t>(val);
		return PyLong_FromLongLong(repr);
	}

	inline static void* from_binding(PyObject* obj)
	{
		size_t repr = PyLong_AsLongLong(obj);
		return reinterpret_cast<void*>(repr);
	}

	static const char* binding_fullname()
	{
		return "long";
	}
};

template<>
struct binding_converter<char>
{
	inline static PyObject* to_binding(char val)
	{				
		return PyLong_FromLong(val);
	}

	inline static char from_binding(PyObject* val)
	{
		long l = PyLong_AsLong(val);
		return (char)l;
	}
	static const char* binding_fullname()
	{
		return "char";
	}
};


template<typename T>
struct binding_converter<std::function<T>>
{	
	using return_type = function_traits<T>::return_type;
	using args_type = function_traits<T>::args_type;
	using func_type = function_traits<T>::func_type;

	inline static PyObject* to_binding(std::function<T> func)
	{
		return nullptr;		
	}

	inline static auto from_binding(PyObject* val)
	{
		constexpr int arg_size = function_traits<T>::args_count;
		func_type res;
		if constexpr (arg_size == 0)
		{
			res = std::bind(&pymethod_wrap<T>::invoke, val);
			return res;
		}
		else if constexpr (arg_size == 1)
		{
			res = std::bind(&pymethod_wrap<T>::invoke, val, std::placeholders::_1);
			return res;
		}
		else if constexpr (arg_size == 2)
		{
			res = std::bind(&pymethod_wrap<T>::invoke, val, std::placeholders::_1, std::placeholders::_2);
			return res;
		}
		else if constexpr (arg_size == 3)
		{
			res = std::bind(&pymethod_wrap<T>::invoke, val, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
			return res;
		}
		return res;
	}
	static const char* binding_fullname()
	{
		return "func";
	}
};


template<typename T>
struct binding_converter<std::vector<T>>
{
	inline static PyObject* to_binding(const std::vector<T>& val)
	{
		PyObject* res = PyList_New(val.size());
		for (auto idx = 0; idx < val.size(); idx++)
		{
			PyObject* element = binding_converter<T>::to_binding(val[idx]);
			PyList_SetItem(res, idx, element);
		}
		return res;
	}

	inline static std::vector<T> from_binding(PyObject* val)
	{
		return std::vector<T>();
	}

	static const char* binding_fullname()
	{
		return LString::Format("List[{0}]", static_type<T>::StaticType()->GetBindingFullName());
	}
};

template<>
struct binding_converter<const char*>
{
	inline static PyObject* to_binding(const char* val)
	{
		return PyUnicode_FromString(val);
	}

	inline static const char* from_binding(PyObject* val)
	{
		if (PyUnicode_Check(val))
		{
			Py_ssize_t size;
			const char* res = PyUnicode_AsUTF8AndSize(val, &size);
			return res;
		}
		return "";		
	}
	static const char* binding_fullname()
	{
		return "str";
	}
};

template<>
struct binding_converter<LString>
{
	inline static PyObject* to_binding(const LString& val)
	{
		return PyUnicode_FromStringAndSize(val.c_str(), val.Length());
	}

	inline static LString from_binding(PyObject* val)
	{
		if (PyUnicode_Check(val))
		{
			Py_ssize_t size;
			const char* res = PyUnicode_AsUTF8AndSize(val, &size);
			LString result = LString(res, res + size);
			return result;
		}
		return "";
	}
	static const char* binding_fullname()
	{
		return "str";
	}
};

template<>
struct binding_converter<float>
{
	inline static PyObject* to_binding(float val)
	{
		return PyFloat_FromDouble(val);
	}

	inline static float from_binding(PyObject* obj)
	{
		float res = (float)PyFloat_AsDouble(obj);
		return res;
	}
	static const char* binding_fullname()
	{
		return "float";
	}
};

template<>
struct binding_converter<unsigned int>
{
	inline static PyObject* to_binding(unsigned int val)
	{
		return PyLong_FromLong(val);
	}
	inline static unsigned int from_binding(PyObject* obj)
	{
		unsigned int res = PyLong_AsLong(obj);
		return res;
	}
	static const char* binding_fullname()
	{
		return "int";
	}
};

template<>
struct binding_converter<long>
{
	inline static PyObject* to_binding(long val)
	{
		return PyLong_FromLong(val);
	}
	static const char* binding_fullname()
	{
		return "int";
	}
};

template<>
struct binding_converter<uint64_t>
{
	inline static PyObject* to_binding(uint64_t val)
	{
		return PyLong_FromSize_t(val);
	}
	inline static uint64_t from_binding(PyObject* val)
	{
		return PyLong_AsSize_t(val);
	}
	static const char* binding_fullname()
	{
		return "long";
	}
};

template<>
struct binding_converter<PyObject*>
{
	inline static PyObject* to_binding(PyObject* val)
	{
		return val;
	}

	inline static PyObject* from_binding(PyObject* obj)
	{
		return obj;
	}

	inline static const char* binding_fullname()
	{
		return "object";
	}
};

template<>
struct binding_converter<LVector3f> : struct_converter<LVector3f> { };

template<>
struct binding_converter<LVector2f> : struct_converter<LVector2f> { };

template<>
struct binding_converter<LQuaternion> : struct_converter<LQuaternion> { };

template<size_t Count, typename First>
void pack_binding_args(PyObject* tup, size_t idx, First first_arg)
{
	if (idx == Count)
		return;
	PyObject* arg = to_binding<First>(std::forward<First>(first_arg));
	PyTuple_SetItem(tup, idx, arg);
}

template<size_t Count, typename First, typename... Args>
void pack_binding_args(PyObject* tup, size_t idx, First first_arg, Args&&... args)
{
	if (idx == Count)
		return;
	PyObject* arg = to_binding<First>(std::forward<First>(first_arg));
	PyTuple_SetItem(tup, idx, arg);

	pack_binding_args<Count>(tup, idx + 1, binding_converter<std::remove_cvref_t<Args>>::to_binding(args)...);
}

template<typename Ret, typename... Args>
struct pymethod_wrap<Ret(Args...)>
{
	static Ret invoke(PyObject* method, Args... args)
	{
		constexpr size_t arg_count = sizeof...(args);
		PyObject* tup = PyTuple_New(arg_count);
		pack_binding_args<sizeof...(args)>(tup, 0, std::forward<Args>(args)...);
		Ref<PyObject, KeepRef> ret = PyObject_Call(method, tup, nullptr);
		Py_DECREF(tup);
		if constexpr (std::is_same_v<Ret, void>)
			return;
		else
			return from_binding<Ret>(ret);
	}
};



}

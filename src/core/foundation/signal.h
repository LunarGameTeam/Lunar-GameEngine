#pragma once

#include <functional>
#include "core/foundation/container.h"
#include "core/memory/ptr.h"

namespace luna
{

template<typename RetVal, typename... Param>
class LFunctionList;

template<typename RetVal, typename... Param>
class LFunction
{
	using FunctionType = std::function<RetVal(Param...)>;
public:
	FunctionType _func;
	LWeakPtr<LFunctionList< RetVal, Param...>> _owner;

public:
	~LFunction()
	{
		auto ptr = _owner.lock();
		if (ptr)
		{
			ptr->m_func_list.remove_if([this](auto ref) {
				return ref.lock().get() == this;
			});
		}
	}
};

template<typename RetVal, typename... Param>
class LFunctionList
{
public:
	using FunctionHandleType = LWeakPtr < LFunction<RetVal, Param...> >;

public:
	LList<FunctionHandleType> m_func_list;
};

template<typename RetVal, typename... Param>
class SignalHandle
{
	using FunctionType = std::function<RetVal(Param...)>;
	using SignatureType = LFunction<RetVal, Param...>;
public:
	SignalHandle()
	{
		_signal = MakeShared<LFunctionList<RetVal, Param... >>();
	}

	LSharedPtr<SignatureType> Bind(FunctionType func)
	{
		LSharedPtr<SignatureType> signature = MakeShared<SignatureType>();
		signature->_func = func;
		LWeakPtr< SignatureType> weak = signature;
		signature->_owner = _signal;
		_signal->m_func_list.emplace_back(weak);
		return signature;
	}
	void Remove(SignatureType func)
	{
		_signal.m_signatures.remove(func);
	}
	void BroadCast(Param... param)
	{
		for (LWeakPtr<SignatureType> node : _signal->m_func_list)
		{
			if (!node.expired())
			{
				node.lock()->_func(param...);
			}
		}
	}
	void Clear()
	{
		_signal->m_func_list.clear();
	}

private:
	LSharedPtr<LFunctionList<RetVal, Param...>> _signal;
};

#define SIGNAL(DelegateTypeName,...) SignalHandle<void, __VA_ARGS__> DelegateTypeName;

}
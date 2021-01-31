#pragma once

#include <boost\bind\bind.hpp>
#include <boost\function.hpp>
#include "core/misc/container.h"

template<typename RetVal, typename... Param>
class DelegateBase;

template<typename RetVal, typename... Param>
class DynamicSignature
{
	using FunctionType = boost::function<RetVal(Param...)>;
public:
	FunctionType _func;
	LWeakPtr<DelegateBase< RetVal, Param...>> _owner;
};


template<typename RetVal, typename... Param>
class DelegateBase
{
public:
	using SignatureType = DynamicSignature<RetVal,Param...>;

public:
	LList<LWeakPtr<SignatureType>> m_signatures;
};

template<typename RetVal, typename... Param>
class SignalHandle
{
	using FunctionType = boost::function<RetVal(Param...)>;
	using SignatureType = DynamicSignature<RetVal, Param...>;
public:
	SignalHandle()
	{
		_signal = MakeShared<DelegateBase<RetVal, Param... >>();
	}
	LSharedPtr<SignatureType> Bind(FunctionType func)
	{
		LSharedPtr<SignatureType> signature = MakeShared<SignatureType>();
		signature->_func = func;
		LWeakPtr< SignatureType> weak = signature;
		signature->_owner = _signal;
		_signal->m_signatures.emplace_back(weak);
		return signature;
	}
	void Remove(SignatureType func)
	{
		_signal.m_signatures.remove(func);
	}
	void BroadCast(Param... param)
	{
		for (LWeakPtr<SignatureType> node : _signal->m_signatures)
		{
			if (!node.expired())
			{
				node.lock()->_func(param...);
			}
		}
	}
	void Clear()
	{
		_signal->m_signatures.clear();
	}

private:
	LSharedPtr<DelegateBase<RetVal, Param...>> _signal;
};


#define SIGNAL_NO_PARAMS(DelegateTypeName,RetVal) SignalHandle<RetVal> DelegateTypeName;
#define SIGNAL_ONE_PARAMS(DelegateTypeName,RetVal,Param1) SignalHandle<RetVal,Param1> DelegateTypeName;
#define SIGNAL_TWO_PARAMS(DelegateTypeName,RetVal,Param1, Param2) SignalHandle<RetVal,Param1,Param2> DelegateTypeName;
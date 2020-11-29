#pragma once

#include <boost\bind\bind.hpp>
#include <boost\function.hpp>
#include "core/misc/container.h"


template<typename RetVal,typename... Param>
class StaticSignature
{
	using Signature = RetVal(*)(Param...); 
	RetVal Invoke( Param... param)
	{
		_func(param...)
	}
	Signature _func;
};


template<typename RetVal, typename Class, typename... Param>
class DynamicSignature
{
	using FuncType = RetVal(Class:: *)(Param...);

public:

	void Bind(Class *pointer, FuncType func)
	{
		_pointer = pointer;
		_func = func;
	}
	RetVal Invoke( Param... param)
	{
		(_pointer->*_func)(param...);
	}
	bool operator==(DynamicSignature &right)
	{
		return this->_pointer == right._pointer && this->_func == right._func;
	}

protected:
	Class *_pointer;
	FuncType _func;
};


template< typename Class, typename RetVal, typename... Param>
class DelegateBase
{
public:
	using SignatureType = DynamicSignature<RetVal,Class,Param...>;
	using MemberFunc = RetVal(Class:: *)(Param...);
	using StaticFunc = RetVal(*)(Param...);

	void BroadCast(Param... param)
	{
		for (auto node : m_signatures)
		{
			node->Invoke(param...);
		}
	}
	void Clear()
	{
		m_signatures.clear();
	}

protected:
	LList<LSharedPtr<SignatureType>> m_signatures;
};

template<typename Class, typename RetVal, typename... Param>
class MultiDynamicDelegate : public DelegateBase<Class, RetVal, Param...>
{
public:
	LWeakPtr<SignatureType> Bind(MemberFunc memberfunc, Class *cls)
	{
		LSharedPtr<SignatureType> signature_ptr = boost::make_shared<SignatureType>();
		signature_ptr->Bind(cls, memberfunc);
		m_signatures.emplace_back(signature_ptr);
		return LWeakPtr<SignatureType>(signature_ptr);
	}
	void Remove(SignatureType func)
	{
		m_signatures.remove(func);
	}
};


#define DELEGATE_NO_PARAMS(DelegateTypeName,ClassType,RetVal) using DelegateTypeName = MultiDynamicDelegate<ClassType,RetVal>;
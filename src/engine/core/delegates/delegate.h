#pragma once

#include <boost\bind\bind.hpp>
#include <boost\function.hpp>
#include <boost\container\list.hpp>

using namespace boost::placeholders;

template<typename RetVal,typename... Param>
class StaticSignature
{
	using FuncType = RetVal(*)(Param...); 
	RetVal Invoke( Param... param)
	{
		_func(param...)
	}
	FuncType _func;
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

protected:
	Class *_pointer;
	FuncType _func;
};


template< typename Class, typename RetVal, typename... Param>
class DelegateBase
{
public:
	using FuncType = DynamicSignature<RetVal,Class,Param...>;
	using MemberFunc = RetVal(Class:: *)(Param...);
	using StaticFunc = RetVal(*)(Param...);

	void BroadCast(Param... param)
	{
		for (auto node : _FunctionList)
		{
			node.Invoke(param...);
		}
	}
	void Clear()
	{
		_FunctionList.clear();
	}

protected:
	boost::container::list<FuncType> _FunctionList;
};

template<typename Class, typename RetVal, typename... Param>
class MultiDynamicDelegate : public DelegateBase<Class, RetVal, Param...>
{
public:
	void Bind(MemberFunc memberfunc, Class *cls)
	{
		FuncType func;
		func.Bind(cls, memberfunc);
		_FunctionList.emplace_back(func);
	}

};


#define DELEGATE_NO_PARAMS(DelegateTypeName,ClassType,RetVal) using DelegateTypeName = MultiDynamicDelegate<ClassType,RetVal>;
#pragma once

#include "core/memory/ptr.h"

//有时间再造轮子，没时间就用boost的容器了

#include <unordered_set>
template<typename T>
using LUnorderedSet = std::unordered_set<T>;

#include <vector>
template<typename T>
using LVector = std::vector<T>;

#include <utility>
template<typename T1, typename T2>
using LPair = std::pair<T1,T2>;

#include <unordered_map>
template<typename K, typename Value>
using LUnorderedMap = std::unordered_map<K, Value>;

#include <map>
template<typename K, typename Value>
using LMap = std::map<K, Value>;

#include <list>
template<typename Value>
using LList = std::list<Value>;

#include <queue>
template<typename Value>
using LQueue = std::queue<Value>;

#include <xhash>
template<typename K>
using LHash = std::hash<K>;

#include <set>
template<typename K>
using LSet = std::set<K>;

namespace luna
{

class LType;
class LObject;

class CORE_API SubPtrArray
{

};
template<typename T, typename U>
class DoubleConverter
{
public:
	DoubleConverter(std::initializer_list<std::pair<T,U>> list)
	{
		for (auto& it : list)
		{
			mData1[it.first] = it.second;
			mData2[it.second] = it.first;
		}
	}
	U Get(const T& t)
	{
		return mData1[t];
	}

	T Get(const U& u)
	{
		return mData2[u];
	}

	std::map<T, U> mData1;
	std::map<U, T> mData2;

};

template<typename T>
class TSubPtrArray
{
private:
	luna::LObject *m_parent = nullptr;
	std::vector<TSubPtr<T>> m_container;
public:

	inline std::vector<TSubPtr<T>>::iterator begin()
	{
		return m_container.begin();
	}

	inline std::vector<TSubPtr<T>>::iterator end()
	{
		return m_container.end();
	}

	inline std::vector<TSubPtr<T>>::const_iterator begin() const
	{
		return m_container.begin();
	}

	inline std::vector<TSubPtr<T>>::const_iterator end() const
	{
		return m_container.end();
	}

	TSubPtrArray()
	{
		assert(0);
	}

	explicit TSubPtrArray(LObject *parent) :
		m_parent(parent)
	{

	}

	void operator=(const TSubPtrArray<T>& rv)
	{
		m_container = rv.m_container;
	}

	void PushBack(T *val)
	{
		m_container.emplace_back(m_parent).SetPtr(val);
	}

	void Erase(T* val)
	{
		for (auto it = m_container.begin(); it != m_container.end(); it++)
		{
			if (it->Get() == val)
			{
				m_container.erase(it);
				break;
			}
		}
		
	}

	size_t Size() const
	{
		return m_container.size();
	}

	T *operator[](size_t index) const
	{
		return m_container[index].Get();
	}

};


}
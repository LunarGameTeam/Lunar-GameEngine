#pragma once
#include "Core/CoreConfig.h"


//有时间再造轮子，没时间就用boost的容器了

#include <unordered_set>
template<typename T>
using LUnorderedSet = std::unordered_set<T>;

#include <vector>
template<typename T>
using LArray = std::vector<T>;

#include <utility>
template<typename T1, typename T2>
using LPair = std::pair<T1,T2>;

#include <unordered_map>
template<typename K, typename Value>
using LUnorderedMap = std::unordered_map<K, Value>;

#include <map>
#include <mutex>
template<typename K, typename Value>
using LMap = std::map<K, Value>;

template<typename Key, typename Val>
class LThreadSafeMap
{
public:
	typedef typename std::map<Key, Val>::iterator this_iterator;
	typedef typename std::map<Key, Val>::const_iterator this_const_iterator;
	Val& operator [](const Key& key)
	{
		std::lock_guard<std::mutex> lk(mtx_);
		return dataMap_[key];
	}

	size_t erase(const Key& key)
	{
		std::lock_guard<std::mutex> lk(mtx_);
		return dataMap_.erase(key);
	}
	this_iterator find(const Key& key)
	{
		std::lock_guard<std::mutex> lk(mtx_);
		return dataMap_.find(key);
	}
	this_const_iterator find(const Key& key) const
	{
		std::lock_guard<std::mutex> lk(mtx_);
		return dataMap_.find(key);
	}

	bool contains(const Key& key) const
	{
		return dataMap_.contains(key);
	}

	this_iterator begin()
	{
		return dataMap_.begin();
	}

	this_const_iterator begin() const
	{
		return dataMap_.begin();
	}

	this_iterator end()
	{
		return dataMap_.end();
	}

	this_const_iterator end() const
	{
		return dataMap_.end();
	}

private:
	std::map<Key, Val> dataMap_;
	std::mutex mtx_;
};

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

class CORE_API PPtrArray
{

};
template<typename T, typename U>
class DoubleConverter
{
public:
	DoubleConverter()
	{

	}

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

	void Set(const T& t, const U& u)
	{
		mData1[t] = u;
		mData2[u] = t;
	}

	T Get(const U& u)
	{
		return mData2[u];
	}

	std::map<T, U> mData1;
	std::map<U, T> mData2;

};

struct HoldIdItem
{
	uint64_t mID = -1;
};

template<typename Value>
class LHoldIdArray
{
	LQueue<size_t> mEmptyIndex;
	LUnorderedMap<size_t, LSharedPtr<Value>> mItems;
public:
	LHoldIdArray()
	{

	}

	Value* AddNewValue()
	{
		size_t newIndex = mItems.size();
		if (!mEmptyIndex.empty())
		{
			newIndex = mEmptyIndex.front();
			mEmptyIndex.pop();
		}
		LSharedPtr<Value> newValue = MakeShared<Value>();
		HoldIdItem* pointer = newValue.get();
		pointer->mID = newIndex;
		mItems.insert({ newIndex ,newValue });
	};

	bool DestroyValue(Value* valueData)
	{
		HoldIdItem* pointer = valueData;
		size_t index = pointer->mID;
		auto itor = mItems.find(index);
		if (itor == mItems.end())
		{
			return false;
		}
		mEmptyIndex.push(index);
		mItems.erase(index);
	}

	void GetAllValueList(LArray<Value*> &valueOut) const
	{
		valueOut.clear();
		for (auto& eachItemData : mItems)
		{
			valueOut.push_back(eachItemData.second.get());
		}
	}

};
}
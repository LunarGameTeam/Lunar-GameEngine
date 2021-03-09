#pragma once

//有时间再造轮子，没时间就用boost的容器了
#define LContainerPack boost

#include "boost/unordered_set.hpp"
template<typename T>
using LUnorderedSet = boost::unordered_set<T>;

#include "boost/container/vector.hpp"
template<typename T>
using LVector = boost::container::vector<T>;

#include "boost/unordered_map.hpp"
#include <unordered_map>
template<typename K, typename Value>
using LUnorderedMap = std::unordered_map<K, Value>;

#include <map>
template<typename K, typename Value>
using LMap = std::map<K, Value>;

#include "boost/container/list.hpp"
template<typename Value>

using LList= boost::container::list<Value>;
template<typename K>
using LHash = boost::hash<K>;


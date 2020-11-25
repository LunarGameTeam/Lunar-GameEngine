#pragma once

//有时间再造轮子，没时间就用boost的容器了

#include "boost/unordered_set.hpp"
template<typename T>
using LUnorderedSet = boost::unordered_set<T>;

#include "boost/container/vector.hpp"
template<typename T>
using LVector = boost::container::vector<T>;

#include "boost/unordered_map.hpp"
template<typename K, typename Value>
using LUnorderedMap = boost::unordered_map<K, Value>;

#include "boost/container/map.hpp"
template<typename K, typename Value>
using LMap = boost::container::map<K, Value>;


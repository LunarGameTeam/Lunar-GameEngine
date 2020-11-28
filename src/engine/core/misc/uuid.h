#pragma once

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
//todo:需要重载等于判定以及其他hash函数，作为键值
using LUuid = boost::uuids::uuid;

LUuid GenerateUUID();

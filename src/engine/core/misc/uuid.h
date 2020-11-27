#pragma once

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
//todo:需要重载等于判定以及其他hash函数，作为键值
class LUuid
{
public:
	static LUuid GenerateUUID();
	boost::uuids::uuid Get()
	{
		return _uid;
	}
protected:
	LUuid(const boost::uuids::uuid& uid);
private:
	boost::uuids::uuid _uid;

};


#pragma once

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>

class LUuid
{
public:
	static LUuid GenerateUUID();
protected:
	LUuid(const boost::uuids::uuid& uid);

private:
	boost::uuids::uuid _uid;

};


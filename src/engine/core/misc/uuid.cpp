#include "uuid.h"



LUuid LUuid::GenerateUUID()
{
	auto uid = boost::uuids::random_generator()();
	return LUuid(uid);
}

LUuid::LUuid(const boost::uuids::uuid &uid) :_uid(uid)
{

}

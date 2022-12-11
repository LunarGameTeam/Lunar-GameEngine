#include "uuid.h"

LUuid GenerateUUID()
{
	auto uid = boost::uuids::random_generator()();
	return uid;
}
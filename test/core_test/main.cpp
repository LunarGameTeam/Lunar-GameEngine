#include "Core/CoreMin.h"
#include "Core/Platform/PlatformModule.h"
#include "Core/Framework/LunaCore.h"
#include "Core/Asset/AssetModule.h"
#include "Core/Event/EventModule.h"

#include <gtest/gtest.h>

using namespace luna;

class TestObject;

class TestObject
{
public:
	void print_sum()
	{
		std::cout << "print" << std::endl;
	}
	void print_sum2(int val)
	{
		std::cout << val << std::endl;
	}
	SIGNAL(TestEvent);
	SIGNAL(TestEvent2, int);
};


#undef main

int main(int argc, char **argv)
{
	testing::InitGoogleTest();
	LunaCore::Ins();
	int result = RUN_ALL_TESTS();
	LunaCore::Ins()->MainLoop();	
	return 1;
}
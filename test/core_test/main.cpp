#include "core/core_min.h"
#include "core/platform/platform_module.h"
#include "core/framework/application.h"
#include "core/asset/asset_module.h"
#include "core/event/event_module.h"

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


class TestApp : public LApplication
{
public:
	TestApp()
	{

	}

};

#undef main

int main(int argc, char **argv)
{
	testing::InitGoogleTest();
	TestApp game;
	game.Run();
	int result = RUN_ALL_TESTS();
	game.MainLoop();

	return Py_BytesMain(argc, argv);
	return 1;
}
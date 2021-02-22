#include "core/core_module.h"
#include "core/file/file_subsystem.h"
#include "core/asset/asset_subsystem.h"
#include "window/window_subsystem.h"
#include "core/event/event_subsystem.h"
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
	SIGNAL(TestEvent2,int);

};


TEST(DelegateTest, Test0)
{
	{
		TestObject* test = new TestObject();	
		auto func = boost::bind(&TestObject::print_sum, test);
		auto func2 = boost::bind(&TestObject::print_sum2, test, boost::placeholders::_1);
		{
			auto handle = test->TestEvent.Bind(func);
			delete test;
		}
	}
}

using namespace luna;

class TestApp
{

public:
	TestApp()
	{
		gEngine->RegisterSubsystem<WindowSubsystem>();
	}
};

#undef main

int main(int argc, const char *argv[])
{
	TestApp game;
	game.Run();
	testing::InitGoogleTest();
	int result = RUN_ALL_TESTS();
	game.MainLoop();
	return 1;
}
#include "core/core_module.h"
#include "core/file/file_subsystem.h"
#include "core/asset/asset_subsystem.h"
#include "core/window/window_subsystem.h"
#include "core/window/event_subsystem.h"
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
	SIGNAL_NO_PARAMS(TestEvent, void);
	SIGNAL_ONE_PARAMS(TestEvent2, void, int);

};


TEST(DelegateTest, Test0)
{
	auto h =
	{
		TestObject test;	
		auto func = boost::bind(&TestObject::print_sum, &test);
		auto func2 = boost::bind(&TestObject::print_sum2, &test, boost::placeholders::_1);
		auto handle = test.TestEvent.Bind(func);
		{
			auto handle2 = test.TestEvent2.Bind(func2);
		}
		test.TestEvent.BroadCast();
		test.TestEvent2.BroadCast(1);
	}

	
}

int main(int argc, const char* argv[])
{
	gEngine = new lunaCore();
	gEngine->RegisterSubsystem<FileSubsystem>();
	gEngine->RegisterSubsystem<AssetSubsystem>();
	gEngine->RegisterSubsystem<WindowSubsystem>();
	gEngine->RegisterSubsystem<EventSubsystem>();
	gEngine->Run();
	testing::InitGoogleTest();
	int result = RUN_ALL_TESTS();
	gEngine->MainLoop();
	return result;
}
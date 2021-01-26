#include "core/core_module.h"
#include "core/file/file_subsystem.h"
#include "core/asset/asset_subsystem.h"
#include "core/window/window_subsystem.h"
#include "core/window/event_subsystem.h"
#include <gtest/gtest.h>

using namespace luna;

// 
// TEST(DelegateTest, Test0)
// {
// 
// 	class TestObject
// 	{
// 	public:
// 		void print_sum()
// 		{
// 			std::cout << "print";
// 		}
// 
// 	};
// 
// 	DELEGATE_NO_PARAMS(TestEvent, TestObject, void);
// 	TestEvent evet;
// 	TestObject test;
// 	evet.Bind(&TestObject::print_sum, &test);
// 	evet.BroadCast();
// }

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
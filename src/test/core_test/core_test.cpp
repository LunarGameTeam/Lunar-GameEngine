#include "core/core_module.h"
#include "core/file/path.h"
#include "core/file/file_subsystem.h"

#include <boost/function.hpp>
#include <boost/bind.hpp>
#include "core/config/config.h"

using namespace luna;

#include <gtest/gtest.h>



TEST(DelegateTest, Test0)
{

	class TestObject
	{
	public:
		void print_sum()
		{
			std::cout << "print";
		}

	};

	DELEGATE_NO_PARAMS(TestEvent, TestObject, void);
	TestEvent evet;
	TestObject test;
	evet.Bind(&TestObject::print_sum,&test);
	evet.BroadCast();
}


TEST(GameEngine, Test0)
{

	gEngine = new LunarEngineCore();
	gEngine->RegisterSubsystem<FileSubsystem>();
	gEngine->Run();
	LString dir = gEngine->GetSubsystem<FileSubsystem>()->GetPlatformFileManager()->EngineDir();
	LString str = "/log/log.txt";
	LString split = "log";
	LPath path("/log/log.txt");	
}


TEST(GameEngine, String)
{	
	LString str = ToString(1);
	int i = FromString<int>(str);
	auto f = ToString(1.1f);
}

int main(int argc, const char* argv[])
{
	float f = FromString<float>("1.1");
	ConfigManager::instance();
	LogVerboseFormat(E_Core, "%f", f);
	testing::InitGoogleTest();
	return RUN_ALL_TESTS();
}
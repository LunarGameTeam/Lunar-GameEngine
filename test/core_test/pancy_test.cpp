/*
* 
#include "Core/core_module.h"
#include "Core/Platform/path.h"
#include "Core/Platform/file_subsystem.h"

#include <boost/function.hpp>
#include <boost/bind.hpp>
#include "Core/Foundation/config.h"
#include "Core/Asset/lasset.h"
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

	gEngine = new lunaCore();
	gEngine->RegisterSubsystem<FileSubsystem>();
	gEngine->Run();
	LString dir = gEngine->GetSubsystem<FileSubsystem>()->GetPlatformFileManager()->EngineDir();
	LString str = "/Foundation/log.txt";
	LString split = "log";
	LPath path("/Foundation/log.txt");	
}


TEST(GameEngine, String)
{	
	LString str = ToString(1);
	int i = FromString<int>(str);
	auto f = ToString(1.1f);
}
//测试assert及ptr
struct CeshiAssertDesc
{
	int a;
	int b;
	std::string buffer_data_file;
};
INIT_REFLECTION_CLASS(CeshiAssertDesc,
	reflect_data.a,
	reflect_data.b,
	reflect_data.buffer_data_file
);
class Cweshi :public LTemplateAsset<CeshiAssertDesc>
{
public:
	Cweshi(const luna::LString& resource_name_in, const CeshiAssertDesc& assert_desc) :LTemplateAsset<CeshiAssertDesc>(resource_name_in, assert_desc)
	{
	};
	Cweshi(const luna::LString& resource_name_in, const Json::Value& resource_desc) :LTemplateAsset<CeshiAssertDesc>(resource_name_in, resource_desc)
	{
	};
	Cweshi(const luna::LString& resource_name_in, const void* resource_desc, const size_t& resource_size) :LTemplateAsset<CeshiAssertDesc>(resource_name_in, resource_desc, resource_size)
	{
	};
	void CheckIfLoadingStateChanged(LLoadState& m_object_load_state) override
	{
		m_object_load_state = LLoadState::LOAD_STATE_FINISHED;
	};
	virtual luna::LResult InitResorceByDesc(const CeshiAssertDesc& resource_desc) override
	{
		int a = 0;
		return luna::g_Succeed;
	}
};
Cweshi* poinetr_this;
void test_gc_ceshi()
{
	Json::Value now_json_value;
	luna::LResult check_error = PancyJsonTool::GetInstance()->LoadJsonFile("111.json", now_json_value);
	poinetr_this = LCreateAssetByJson<Cweshi>(luna::LString("222.json"), now_json_value,true);
	LPtr<Cweshi> ceshi(poinetr_this);
	ceshi = LCreateAssetByJson<Cweshi>(luna::LString("111.json"), now_json_value, true);
	auto ceshi1 = ceshi.Get();
}
//测试固定大小线性增长的表数据结构
LInitNewFastIdClass(LinerGrowIndexMapID, int32_t);
class LinerGrowMapTest1;
class LinerGrowNodeTest1 :public luna::LinearGrownResourceData<int32_t>
{
public:
	LinerGrowNodeTest1() : luna::LinearGrownResourceData<int32_t>()
	{

	};
	~LinerGrowNodeTest1()
	{

	}
	LResult InitResource(const ValueMarkIndex<int32_t>& data_index) override
	{
		int a = 0;
		return luna::g_Succeed;
	};
};
class LinerGrowListTest1 :public luna::LinerGrowListMemberOnlyOne<int32_t>
{
public:
	LinerGrowListTest1(
		const int32_t& list_index,
		const int32_t& max_list_size,
		LLinearGrowMap<int32_t>* controler_map
	):luna::LinerGrowListMemberOnlyOne<int32_t>(list_index, max_list_size, controler_map)
	{
	};
	~LinerGrowListTest1()
	{
	};
private:
	LResult BuildNewValue(const ValueMarkIndex<int32_t>& value_index) override
	{
		int a = 0;
		return luna::g_Succeed;
	};
	LResult ReleaseValue(const int32_t& value_index) override
	{
		int a = 0;
		return luna::g_Succeed;
	};
};
class LinerGrowMapTest1 :public LLinearGrowMap<int32_t>
{
	luna::LSafeIndexType::LinerGrowIndexMapIDIndexGennerator uuid_generate;
public:
	LinerGrowMapTest1(const int32_t& max_size_per_list):LLinearGrowMap<int32_t>(max_size_per_list)
	{
	};
	~LinerGrowMapTest1()
	{
	};
private:
	virtual luna::LResult BuildNewListToMap(const int32_t& list_index, const int32_t& max_size_per_list, ILinearGrowListMember<int32_t>*& resource_list_pointer)override
	{
		resource_list_pointer = new LinerGrowListTest1(list_index, max_size_per_list, this);
		return luna::g_Succeed;
	};
	int32_t GenerateNewListValue() override
	{
		return uuid_generate.GetUuid().GetValue();
	};
	luna::LResult OnReleaseListFromMap(const int32_t& index, ILinearGrowListMember<int32_t>* resource_list_pointer)override
	{
		delete resource_list_pointer;
		return luna::g_Succeed;
	};
};
void TestLinerGrownTable()
{
	LinerGrowNodeTest1* ceshi_resource[1000];
	LinerGrowMapTest1 ceshi_liner_grown(10);
	for (int i = 0; i < 1000; ++i)
	{
		ceshi_resource[i] = new LinerGrowNodeTest1();
		ceshi_liner_grown.AllocatedDataFromMap(1, ceshi_resource[i]);
	}
	delete ceshi_resource[999];
	delete ceshi_resource[998];
	delete ceshi_resource[997];
	delete ceshi_resource[996];
	delete ceshi_resource[995];
	delete ceshi_resource[994];

	delete ceshi_resource[969];
	delete ceshi_resource[968];
	delete ceshi_resource[967];
	delete ceshi_resource[966];
	delete ceshi_resource[965];
	delete ceshi_resource[964];
	delete ceshi_resource[963];
	delete ceshi_resource[962];
	delete ceshi_resource[961];
	LinerGrowNodeTest1 ceshi_array[100];
	ceshi_liner_grown.AllocatedDataFromMap(1, &ceshi_array[0]);
	delete ceshi_resource[971];
	ceshi_liner_grown.AllocatedDataFromMap(1, &ceshi_array[1]);
	for (int i = 2; i < 20; ++i)
	{
		ceshi_liner_grown.AllocatedDataFromMap(1, &ceshi_array[i]);
	}
}
//测试随机大小线性增长表数据结构
LInitNewFastIdClass(LinerGrowIndexMapID2, size_t);
class LinerGrowMapTest2;
class LinerGrowNodeTest2 :public luna::LinearGrownResourceData<size_t>
{
public:
	LinerGrowNodeTest2() : luna::LinearGrownResourceData<size_t>()
	{

	};
	~LinerGrowNodeTest2()
	{

	}
	LResult InitResource(const ValueMarkIndex<size_t>& data_index) override
	{
		int a = 0;
		return luna::g_Succeed;
	};
};
class LinerGrowListTest2 :public luna::LinerGrowListMemberCommon<size_t>
{
public:
	LinerGrowListTest2(
		const size_t& list_index,
		const size_t& max_list_size,
		LLinearGrowMap<size_t>* controler_map
	) :luna::LinerGrowListMemberCommon<size_t>(list_index, max_list_size, controler_map)
	{
	};
	~LinerGrowListTest2()
	{
	};
private:
	LResult BuildNewValue(const ValueMarkIndex<size_t>& value_index) override
	{
		int a = 0;
		return luna::g_Succeed;
	};
	LResult ReleaseValue(const size_t& value_index) override
	{
		int a = 0;
		return luna::g_Succeed;
	};
};
class LinerGrowMapTest2 :public LLinearGrowMap<size_t>
{
	luna::LSafeIndexType::LinerGrowIndexMapIDIndexGennerator uuid_generate;
public:
	LinerGrowMapTest2(const size_t& max_size_per_list) :LLinearGrowMap<size_t>(max_size_per_list)
	{
	};
	~LinerGrowMapTest2()
	{
	};
private:
	virtual luna::LResult BuildNewListToMap(const size_t& list_index, const size_t& max_size_per_list, ILinearGrowListMember<size_t>*& resource_list_pointer) override
	{
		resource_list_pointer = new LinerGrowListTest2(list_index, max_size_per_list, this);
		return luna::g_Succeed;
	};
	size_t GenerateNewListValue() override
	{
		return uuid_generate.GetUuid().GetValue();
	};
	luna::LResult OnReleaseListFromMap(const size_t& index, ILinearGrowListMember<size_t>* resource_list_pointer) override
	{
		delete resource_list_pointer;
		return luna::g_Succeed;
	};
};
void TestLinerGrownTable2()
{
	LinerGrowNodeTest2* ceshi_resource[1000];
	LinerGrowMapTest2 ceshi_liner_grown(100);
	for (int i = 0; i < 1000; ++i)
	{
		ceshi_resource[i] = new LinerGrowNodeTest2();
		ceshi_liner_grown.AllocatedDataFromMap(15, ceshi_resource[i]);
	}
	delete ceshi_resource[999];
	delete ceshi_resource[998];
	delete ceshi_resource[997];
	delete ceshi_resource[996];
	delete ceshi_resource[995];
	delete ceshi_resource[994];

	delete ceshi_resource[969];
	delete ceshi_resource[968];
	delete ceshi_resource[967];
	delete ceshi_resource[966];
	delete ceshi_resource[965];
	delete ceshi_resource[964];
	delete ceshi_resource[963];
	delete ceshi_resource[962];
	delete ceshi_resource[961];
	LinerGrowNodeTest2 ceshi_array[100];
	ceshi_liner_grown.AllocatedDataFromMap(1, &ceshi_array[0]);
	delete ceshi_resource[971];
	ceshi_liner_grown.AllocatedDataFromMap(1, &ceshi_array[1]);
	for (int i = 2; i < 20; ++i)
	{
		ceshi_liner_grown.AllocatedDataFromMap(1, &ceshi_array[i]);
	}
}
//测试段式分配器的数据
class LRenderDescriptrMemorySegmentalAllocator;
class LBasicBlockSegmental : public SegmentalMemberValue
{
public:
	LBasicBlockSegmental();
	~LBasicBlockSegmental()
	{
	};
	LResult OnSegmentalCreated(const size_t& data_offset, const size_t& data_size) override;
};
LBasicBlockSegmental::LBasicBlockSegmental()
{
};
LResult LBasicBlockSegmental::OnSegmentalCreated(const size_t& data_offset, const size_t& data_size)
{
	return g_Succeed;
};
class LBasicMemorySegmentalAllocator : public SegmentalAllocatorMap
{
public:
	LBasicMemorySegmentalAllocator(const size_t& heap_control_size) :
		SegmentalAllocatorMap(heap_control_size)
	{
	};
	~LBasicMemorySegmentalAllocator()
	{
	}
private:
	LResult BuildNewSegmentalToMap(const SegmentalIndex& required_segmental_index, SegmentalMemberValue* segmental_value) override;
	LResult ReleaseSegmentalFromMap(const SegmentalIndex& required_segmental_index) override;
};
LResult LBasicMemorySegmentalAllocator::BuildNewSegmentalToMap(const SegmentalIndex& required_segmental_index, SegmentalMemberValue* segmental_value)
{
	return g_Succeed;
};
LResult LBasicMemorySegmentalAllocator::ReleaseSegmentalFromMap(const SegmentalIndex& required_segmental_index)
{
	return g_Succeed;
};
void TestSegmentaTable()
{
	LBasicBlockSegmental* ceshi_resource[1000];
	LBasicMemorySegmentalAllocator ceshi_liner_grown(10000);
	ceshi_liner_grown.Create();
	for (int i = 0; i < 1000; ++i)
	{
		ceshi_resource[i] = new LBasicBlockSegmental();
		ceshi_liner_grown.AllocateNewSegmental(10, ceshi_resource[i]);
	}
	delete ceshi_resource[101];
	delete ceshi_resource[201];
	delete ceshi_resource[203];
	for (int i = 0; i < 100; ++i)
	{
		delete ceshi_resource[i];
		int a = 0;
	}
	for (int i = 200; i >102; --i)
	{
		delete ceshi_resource[i];
		int a = 0;
	}
	int a = 0;

}
int main(int argc, const char* argv[])
{
	TestLinerGrownTable();
	TestLinerGrownTable2();
	//ceshi_liner_grown.ReleaseDataFromMap()
	TestSegmentaTable();
	InitNewStructToReflection(CeshiAssertDesc);
	float f = FromString<float>("1.1");
	ConfigManager::instance();
	LogVerboseFormat(E_Core, "%f", f);
	testing::InitGoogleTest();
	test_gc_ceshi();



	luna::GrabageColloector::GetInstance();
	return RUN_ALL_TESTS();
}
*/
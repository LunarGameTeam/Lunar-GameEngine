#include "core/core_module.h"
#include "core/core_configs.h"
#include "render/render_subsystem.h"
#include "core/config/config.h"
#include "core/assert/lassert.h"
#include "window/window_subsystem.h"

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
class Cweshi :public LTemplateAssert<CeshiAssertDesc>
{
public:
	Cweshi(const LunarEngine::LString& resource_name_in, const CeshiAssertDesc& assert_desc):LTemplateAssert<CeshiAssertDesc>(resource_name_in, assert_desc)
	{
	};
	Cweshi(const LunarEngine::LString& resource_name_in, const Json::Value& resource_desc) :LTemplateAssert<CeshiAssertDesc>(resource_name_in, resource_desc)
	{
	};
	Cweshi(const LunarEngine::LString& resource_name_in, const void* resource_desc, const size_t& resource_size) :LTemplateAssert<CeshiAssertDesc>(resource_name_in, resource_desc, resource_size)
	{
	};
	void CheckIfLoadingStateChanged(LLoadState& m_object_load_state) override
	{
		m_object_load_state = LLoadState::LOAD_STATE_FINISHED;
	};
	virtual LunarEngine::LResult InitResorceByDesc(const CeshiAssertDesc& resource_desc) override
	{
		int a = 0;
		return LunarEngine::g_Succeed;
	}
};
Cweshi* poinetr_this;
void test_gc_ceshi()
{
	Json::Value now_json_value;
	LunarEngine::LResult check_error = PancyJsonTool::GetInstance()->LoadJsonFile("111.json", now_json_value);
	poinetr_this = LCreateAssetByJson<Cweshi>(LunarEngine::LString("222.json"), now_json_value);
	LPtr<Cweshi> ceshi(poinetr_this);
	ceshi = LCreateAssetByJson<Cweshi>(LunarEngine::LString("111.json"), now_json_value);
	auto ceshi1 = ceshi.Get();
}
int main(int argc, const char* argv[])
{
	InitNewStructToReflection(CeshiAssertDesc);
	test_gc_ceshi();
	gEngine = new LunarEngineCore();
	gEngine->RegisterSubsystem<WindowSubusystem>();
	gEngine->RegisterSubsystem<RenderSubusystem>();
	gEngine->Run();
	gEngine->MainLoop();
}
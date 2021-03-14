#include "core/core_module.h"
#include "core/file/path.h"
#include "core/config/config.h"
#include "core/file/file_subsystem.h"
#include "core/asset/asset_subsystem.h"
#include "core/asset/asset.h"

using namespace luna;

#include <gtest/gtest.h>


/*
 *	Text Asset样例
 */
class TextAsset :public LBasicAsset
{

public:
	void OnAssetFileLoad(LSharedPtr<Dictionary> meta, LSharedPtr<LFile> file) override
	{
		const char *begin = (const char*)file->GetData().data();
		const char *end = (const char *)file->GetData().data() + file->GetData().size();
		content = LString(begin, end);
		m_meta = meta;
	}
	LSharedPtr<Dictionary> m_meta;//对应的Meta信息
	LString content;//文件文本内容

};

TEST(Core, LoadAssetSync)
{
	FileSubsystem *file_sys = gEngine->GetSubsystem<FileSubsystem>();
	IPlatformFileManager *manager = file_sys->GetPlatformFileManager();
	AssetSubsystem *asset_sys = gEngine->GetSubsystem<AssetSubsystem>();
	LPath entry_file("/assets/entry.lua");
	LSharedPtr<TextAsset> script = asset_sys->LoadAsset<TextAsset>(entry_file);
	LogVerboseFormat(E_Core, "Asset Content: %s", script->content.c_str());
}

/// <summary>
/// Config测试
/// </summary>
TEST(Core, Config)
{
	//会从config.ini里饭序列化，以及会序列化到config.ini里
}
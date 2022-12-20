#include "core/foundation/config.h"
#include "config.h"
#include <tchar.h>
#include "windows.h"

#include "core/file/platform_module.h"
#include "core/framework/luna_core.h"

#include <boost/algorithm/string.hpp>

namespace luna
{
}

namespace luna
{


using namespace boost::algorithm;

ConfigLoader::ConfigLoader()
{

}

ConfigLoader::~ConfigLoader()
{
}

void ConfigLoader::Load(const LString& val)
{
	std::vector<std::string> lines;
	LString group = "";
	boost::algorithm::split(lines, val.std_str(), boost::is_any_of("\n"));
	for (auto& it : lines)
	{
		LString line = it;
		if (line.StartWith("["))
		{
			line.EraseFirst("[");
			line.EraseLast("]");
			group = line;
		}
		else
		{
			if (line.Contains("="))
			{
				auto pos = line.Find('=');
				LString key = line.Substr(0, pos);
				sConfigs[key].mKey = line.Substr(0, pos);
				sConfigs[key].mValue = line.Substr(pos + 1);
				sConfigs[key].mGroup = group;
			}
		}
	}
}

void ConfigLoader::Save()
{

	std::fstream fs;
	TCHAR tempPath[1000];
	GetCurrentDirectory(MAX_PATH, tempPath); //获取程序的当前目录
	LString path(tempPath);
	path = path + "/config.ini";
	fs.open(path.c_str(), std::fstream::out | std::fstream::trunc);
	LMap<LString, LArray<SerializeConfig>> configs;

	for (auto& it : sConfigs)
	{
		auto& group_config = configs[it.second.mGroup];
		SerializeConfig config
		{
			it.second.mGroup,
			it.second.mKey,
			it.second.mValue
		};
		group_config.push_back(config);
	}

	for (auto& it : configs)
	{
		LString group = LString::Format("[{0}]\n", it.first.c_str());
		fs << group.c_str();
		for (auto& config : it.second)
		{
			fs << config.mKey << "=" << config.mValue << std::endl;
		}
	}
	fs.flush();
	fs.close();
}


}
#include "config.h"

#include <boost/algorithm/string.hpp>
#include <fstream>

#include "core/serialization/dict_data.h"

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

void ConfigLoader::LoadFromJson(const LString& content)
{
	Json::Value val;
	Dictionary::FromLString(content, val);		
	for (auto it = val.begin(); it != val.end(); ++it)
	{
		LString group = it.name();
		for (auto it2 = it->begin(); it2 != it->end(); ++it2) 
		{
			LString key = it2.name();
			if (sConfigs[key] == nullptr)
				sConfigs[key] = new SerializeConfig();
			sConfigs[key]->mKey = it2.name();
			sConfigs[key]->mValue = it2->asCString();
			sConfigs[key]->mGroup = group;
		}
	}
}

void ConfigLoader::SaveJson(std::fstream& fs)
{
	LMap<LString, LArray<SerializeConfig*>> configs;

	for (auto& it : sConfigs)
	{
		auto& groupConfig = configs[it.second->mGroup];
		groupConfig.push_back(it.second);
	}
	Json::Value val;
	for (auto& it : configs)
	{		
		LString group = it.first.c_str();
		Json::Value & valueGroup= val[group.c_str()];		
		
		for (auto config : it.second)
		{
			valueGroup[config->mKey] = config->mValue.c_str();			
		}
	}
	Json::StyledWriter fastWriter;
	LString Content = fastWriter.write(val);
	fs << Content.c_str();
	fs.flush();
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
				if (sConfigs[key] == nullptr)
					sConfigs[key] = new SerializeConfig();
				sConfigs[key]->mKey = line.Substr(0, pos);
				sConfigs[key]->mValue = line.Substr(pos + 1);
				sConfigs[key]->mGroup = group;
			}
		}
	}
}

void ConfigLoader::Save(std::fstream& fs)
{

	LMap<LString, LArray<SerializeConfig*>> configs;

	for (auto& it : sConfigs)
	{
		auto& groupConfig = configs[it.second->mGroup];
		groupConfig.push_back(it.second);
	}
	for (auto& it : configs)
	{
		LString group = LString::Format("[{0}]\n", it.first.c_str());
		fs << group.c_str();
		for (auto config : it.second)
		{
			fs << config->mKey << "=" << config->mValue << std::endl;
		}
	}
	fs.flush();
}


}
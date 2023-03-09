#pragma once

#include "Graphics/Asset/ShaderAsset.h"
#include "Core/Math/Math.h"
#include "Core/Foundation/String.h"
#include "Core/Asset/AssetModule.h"
#include "Graphics/RenderModule.h"


namespace luna::render
{
RegisterTypeEmbedd_Imp(ShaderAsset)
{
	cls->Binding<Self>();;
	cls->Ctor<ShaderAsset>();
	BindingModule::Get("luna")->AddType(cls);
};

void ShaderAsset::OnAssetFileRead(LSharedPtr<JsonDict> meta, LSharedPtr<LFile> file)
{
	TextAsset::OnAssetFileRead(meta, file);
	Init();
	// 	size_t pos;
	// 	std::vector<std::string> m_lines;
	// 	boost::algorithm::split(m_lines, m_content.str(), boost::is_any_of("\n"));
	// 	std::string res;
	// 	for (std::string &line : m_lines)
	// 	{
	// 		auto pos = line.find("#include");
	// 		if (pos != std::string::npos)
	// 		{
	// 			std::string file = line.substr(pos + 8);
	// 			boost::algorithm::replace_all(file, "\r", "");			
	// 			boost::algorithm::trim(file);
	// 			ShaderAsset* header = g_asset_sys->LoadAsset<ShaderAsset>(file.c_str());
	// 			line = header->m_content.str();
	// 		}
	// 		res += line;
	// 	}
	// 	m_content.Assign(res);		
	// 	
}

void ShaderAsset::Init()
{
	LString content = GetContent();
	LString vertexContent;
	LString fragContent;
	auto pos = content.Find("//Split");
	if (pos == content.npos)
	{
		fragContent = content;
		vertexContent = content;
	}
	else
	{
		vertexContent = content.Substr(0, pos);
		fragContent = content.Substr(pos);
	}
	if (!mInit)
	{
		RHIShaderDesc shaderDesc;
		shaderDesc.mName = GetAssetPath();
		shaderDesc.mContent = vertexContent;
		shaderDesc.mEntryPoint = "VSMain";
		shaderDesc.mType = RHIShaderType::Vertex;
		mVS = sRenderModule->GetRHIDevice()->CreateShader(shaderDesc);
		shaderDesc.mContent = fragContent;
		shaderDesc.mEntryPoint = "PSMain";
		shaderDesc.mType = RHIShaderType::Pixel;
		mPS = sRenderModule->GetRHIDevice()->CreateShader(shaderDesc);
		mInit = true;
	}
	std::vector<RHIBindPoint> bindingKeys;
	std::map<std::tuple<uint32_t, uint32_t>, RHIBindPoint> result;
	for (auto& it : mVS->mBindPoints)
	{
		auto& bindKey = it.second;
		result[std::make_tuple(bindKey.mSpace, bindKey.mSlot)] = bindKey;
	}
	for (auto& it : mPS->mBindPoints)
	{
		auto& bindKey = it.second;
		result[std::make_tuple(bindKey.mSpace, bindKey.mSlot)] = bindKey;
	}
	for (auto it : result)
	{
		bindingKeys.push_back(it.second);
	}
	mLayout = sRenderModule->GetRHIDevice()->CreateBindingSetLayout(bindingKeys);

}
};


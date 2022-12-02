#pragma once

#include "shader_asset.h"
#include "core/math/math.h"
#include "core/misc/string.h"
#include "core/asset/asset_module.h"
#include "render/render_module.h"


namespace luna::render
{
RegisterTypeEmbedd_Imp(ShaderAsset)
{
	cls->Binding<Self>();
	LBindingModule::Get("luna")->AddType(cls);
	cls->Ctor<ShaderAsset>();
};

void ShaderAsset::OnAssetFileRead(LSharedPtr<Dictionary> meta, LSharedPtr<LFile> file)
{
	LTextAsset::OnAssetFileRead(meta, file);
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
}
};


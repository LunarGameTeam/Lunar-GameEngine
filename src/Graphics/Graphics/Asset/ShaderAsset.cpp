#pragma once

#include "Graphics/Asset/ShaderAsset.h"
#include "Core/Math/Math.h"
#include "Core/Foundation/String.h"
#include "Core/Asset/AssetModule.h"
#include "Graphics/RenderModule.h"


namespace luna::graphics
{

RegisterTypeEmbedd_Imp(ShaderMacro)
{
	cls->Ctor<ShaderMacro>();
	cls->Property<&Self::mMacroName>("macro_name")
		.Serialize();
	cls->BindingProperty<&Self::mMacroValue>("macro_value")
		.Serialize();
	BindingModule::Luna()->AddType(cls);
}

RegisterTypeEmbedd_Imp(ShaderAsset)
{
	cls->Binding<Self>();;
	cls->Ctor<ShaderAsset>();
	BindingModule::Get("luna")->AddType(cls);
};

void ShaderAsset::OnAssetFileRead(LSharedPtr<JsonDict> meta, LSharedPtr<LFile> file)
{
	TextAsset::OnAssetFileRead(meta, file);
}

SharedPtr<LShaderInstance> ShaderAsset::GenerateShaderInstance(RHIShaderType shaderType,const LArray<ShaderMacro*>& shaderMacros)
{
	SharedPtr<LShaderInstance> newInstance = MakeShared<LShaderInstance>(shaderType, GetAssetPath());
	newInstance->Init(GetContent(), shaderMacros);
	return newInstance;
}

void LShaderInstance::Init(
	const LString& content,
	LArray<ShaderMacro*> shaderMacros
)
{
	if (!mInit)
	{
		RHIShaderDesc shaderDesc;
		shaderDesc.mContent = content;
		shaderDesc.mName = mShaderName;
		shaderDesc.mType = mType;
		for (ShaderMacro* eachMacro : shaderMacros)
		{
			shaderDesc.mShaderMacros.push_back({ eachMacro->mMacroName, eachMacro->mMacroValue});
		}
		switch (mType)
		{
			case luna::graphics::RHIShaderType::Vertex:
			{
				shaderDesc.mEntryPoint = "VSMain";
			}
			break;
			case luna::graphics::RHIShaderType::Pixel:
			{
				shaderDesc.mEntryPoint = "PSMain";
			}
			break;
			default:
			break;
		}
		mRhiShader = sRenderModule->GetRenderContext()->CreateShader(shaderDesc);
		mShaderCacheId = sRenderModule->GetRenderContext()->GetShaderId(mRhiShader);

		std::vector<RHIBindPoint> bindingKeys;
		std::map<std::tuple<uint32_t, uint32_t>, RHIBindPoint> result;
		for (auto& it : mRhiShader->mBindPoints)
		{
			auto& bindKey = it.second;
			result[std::make_tuple(bindKey.mSpace, bindKey.mSlot)] = bindKey;
		}
		for (auto it : result)
		{
			bindingKeys.push_back(it.second);
		}
		mLayout = sRenderModule->GetRHIDevice()->CreateBindingSetLayout(bindingKeys);
		mInit = true;
	}
}
};

